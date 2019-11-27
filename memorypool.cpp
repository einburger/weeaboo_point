#include "stdafx.h"
#include "memorypool.h"
#include <Windows.h>
#include "memoryapi.h"

void allocate_pool(MemoryPool* pool, size_t size) {
    pool->used_size = 0;
    pool->total_size = size;
	pool->base = (uint8_t *)VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void clear_pool(MemoryPool* pool) {
	pool->used_size = 0;
}

void *push_object_(MemoryPool* pool, size_t size) {
    void *object_base = pool->base + pool->used_size;
    pool->used_size += size;
    return object_base;
}

void pop_object_(MemoryPool* pool, size_t size) {
	pool->used_size -= size;
}

void *get_end_(MemoryPool* pool, size_t size) {
	if (pool->used_size == 0)
		return NULL;
	void* object = pool->base + pool->used_size - size;
	return object;
}

