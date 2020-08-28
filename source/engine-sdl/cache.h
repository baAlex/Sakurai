/*-----------------------------

 [cache.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>

struct Cache;

struct CacheItem
{
	void* ptr;

	// Read only:
	bool marked;
	size_t estimated_size;
	void (*callback_delete)(void*);
};

struct Cache* CacheCreate(size_t max_estimated_size);
void CacheDelete(struct Cache*);

struct CacheItem* CacheAdd(struct Cache*, const char* key, size_t estimated_size, void (*callback_delete)(void*));
struct CacheItem* CacheFind(const struct Cache*, const char* key);

void CacheMarkAll(struct Cache*);
void CacheMark(struct CacheItem*);
void CacheUnmark(struct CacheItem*); // TODO

int CacheTest();

#endif
