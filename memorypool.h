#pragma once

typedef struct MemoryPool
{
	uint8_t*	base;
	size_t		total_size;
	size_t		used_size;
} MemoryPool;

void clear_pool(MemoryPool* pool);

#define PUSH_OBJECT(pool, type) (type *)push_object_(pool, sizeof(type))
void* push_object_(MemoryPool* pool, size_t size);

#define POP_OBJECT(pool, type) pop_object_(pool, sizeof(type))
void pop_object_(MemoryPool* pool, size_t size);

#define GET_END(pool, type) (type *)get_end_(pool, sizeof(type))
void* get_end_(MemoryPool* pool, size_t size);

