#include "core/allocator.h"
#include <utility>

namespace infini
{
    Allocator::Allocator(Runtime runtime) : runtime(runtime)
    {
        used = 0;
        peak = 0;
        ptr = nullptr;

        // 'alignment' defaults to sizeof(uint64_t), because it is the length of
        // the longest data type currently supported by the DataType field of
        // the tensor
        alignment = sizeof(uint64_t);
    }

    Allocator::~Allocator()
    {
        if (this->ptr != nullptr)
        {
            runtime->dealloc(this->ptr);
        }
    }

    size_t Allocator::alloc(size_t size)
    {
        IT_ASSERT(this->ptr == nullptr);
        // pad the size to the multiple of alignment
        size = this->getAlignedSize(size);

        // =================================== 作业 ===================================
        // TODO: 设计一个算法来分配内存，返回起始地址偏移量
        // =================================== 作业 ===================================
        size_t addr = UINT64_MAX;
        for (auto it = free_blocks.begin(); it != free_blocks.end(); it++) {
            if (it->second >= size) {
                addr = it->first;
                free_blocks.erase(it);
                if (it->second > size) {
                    auto block_addr = it->first + size;
                    auto block_size = it->second - size;
                    if (block_size != 0) {
                        free_blocks.insert(std::pair(block_addr, block_size));
                    }
                }
                return addr;
            }
        }

        addr = used;
        used += size;
        peak = used;
        return addr;
    }

    void Allocator::free(size_t addr, size_t size)
    {
        IT_ASSERT(this->ptr == nullptr);
        size = getAlignedSize(size);

        // =================================== 作业 ===================================
        // TODO: 设计一个算法来回收内存
        // =================================== 作业 ===================================
        free_blocks.insert(std::make_pair(addr, size));
        for (auto it = free_blocks.begin(); it != free_blocks.end(); it++) {
            auto next_addr = it->first + it->second;
            auto next_it = next(it);
            if (next_addr == next_it->first) {
                it->second += next_it->second;
                free_blocks.erase(next_it);
            } else if (next_addr == used) {
                used -= it->second;
                free_blocks.erase(it);
                break;
            }
        }
        peak = used;
    }

    void *Allocator::getPtr()
    {
        if (this->ptr == nullptr)
        {
            this->ptr = runtime->alloc(this->peak);
            printf("Allocator really alloc: %p %lu bytes\n", this->ptr, peak);
        }
        return this->ptr;
    }

    size_t Allocator::getAlignedSize(size_t size)
    {
        return ((size - 1) / this->alignment + 1) * this->alignment;
    }

    void Allocator::info()
    {
        std::cout << "Used memory: " << this->used
                  << ", peak memory: " << this->peak << std::endl;
    }
}
