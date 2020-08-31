/*-----------------------------

MIT License

Copyright (c) 2020 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [cache.c]
 - Alexander Brandt 2020
-----------------------------*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "cache.h"
#include "japan-dictionary.h"

#define MARKED_ITEMS 16


struct Cache
{
	struct jaDictionary* d;
	size_t max_size;
	size_t usage_size;

	// To keep track of some items safe to delete:
	size_t marked_no;
	struct jaDictionaryItem* marked[MARKED_ITEMS];
};


static struct CacheItem* CacheFind2(const struct Cache* cache, const char* key, struct jaDictionaryItem** out_item);


void sCallbackDeleteItem(struct jaDictionaryItem* d_item)
{
	struct CacheItem* item = d_item->data;

	printf("Removed item '%s' from cache\n", d_item->key);

	if (item->callback_delete != NULL)
		item->callback_delete(item->ptr);
}


void sCallbackFindMarked(struct jaDictionaryItem* d_item, void* raw_cache)
{
	struct Cache* cache = raw_cache;
	struct CacheItem* item = d_item->data;

	// Add item to the marked array
	if (item->marked == true && cache->marked_no < MARKED_ITEMS)
	{
		for (int i = 0; i < MARKED_ITEMS; i++)
		{
			if (cache->marked[i] == NULL)
			{
				cache->marked[i] = d_item;
				cache->marked_no += 1;
				break;
			}
		}
	}
}


void sCallbackCleanAll(struct jaDictionaryItem* d_item, void* raw_cache)
{
	struct CacheItem* item = d_item->data;

	item->marked = true;
	sCallbackFindMarked(d_item, raw_cache);
}


struct Cache* CacheCreate(size_t max_estimated_size)
{
	struct Cache* cache = NULL;

	if ((cache = calloc(1, sizeof(struct Cache))) != NULL)
	{
		cache->max_size = max_estimated_size;

		if ((cache->d = jaDictionaryCreate(NULL)) == NULL)
		{
			free(cache);
			return NULL;
		}
	}

	return cache;
}


void CacheDelete(struct Cache* cache)
{
	jaDictionaryDelete(cache->d);
	free(cache);
}


void CacheMarkAll(struct Cache* cache)
{
	jaDictionaryIterate(cache->d, sCallbackCleanAll, cache);
}


static int sMakeSpace(struct Cache* cache, size_t required_size)
{
	struct CacheItem* item = NULL;

	if ((cache->usage_size + required_size) <= cache->max_size)
		return 0;

	// Remove marked items
	if (cache->marked_no > 0)
	{
	again:
		for (int i = 0; i < MARKED_ITEMS; i++)
		{
			if (cache->marked[i] != NULL)
			{
				item = cache->marked[i]->data;
				cache->usage_size -= item->estimated_size;

				jaDictionaryRemove(cache->marked[i]);

				cache->marked[i] = NULL;
				cache->marked_no -= 1;
			}

			// That worked?
			if ((cache->usage_size + required_size) <= cache->max_size)
				return 0;
		}
	}

	// Fill the marked array and try again
	jaDictionaryIterate(cache->d, sCallbackFindMarked, cache);

	if (cache->marked_no > 0)
		goto again;

	// No luck
	return 1;
}


struct CacheItem* CacheAdd(struct Cache* cache, const char* key, size_t estimated_size, void (*callback_delete)(void*))
{
	struct jaDictionaryItem* d_item = NULL;
	struct CacheItem* item = NULL;

	if ((item = CacheFind2(cache, key, &d_item)) != NULL)
	{
		if (item->marked == false)
			return NULL;

		if (cache->marked_no > 0)
		{
			for (int i = 0; i < MARKED_ITEMS; i++)
			{
				if (cache->marked[i] == d_item)
				{
					cache->marked[i] = NULL;
					cache->marked_no -= 1;
					break;
				}
			}
		}

		cache->usage_size -= item->estimated_size;
		jaDictionaryRemove(d_item);
	}

	if (sMakeSpace(cache, estimated_size) == 1)
		return NULL;

	if ((d_item = jaDictionaryAdd(cache->d, key, NULL, sizeof(struct CacheItem))) != NULL)
	{
		cache->usage_size += estimated_size;
		d_item->callback_delete = sCallbackDeleteItem;

		item = d_item->data;
		item->marked = false;
		item->estimated_size = estimated_size;
		item->callback_delete = callback_delete;
	}

	printf("New item '%s' in cache\n", key);
	return item;
}


struct CacheItem* CacheFind(const struct Cache* cache, const char* key)
{
	struct jaDictionaryItem* d_item = jaDictionaryGet(cache->d, key);
	return (d_item == NULL) ? NULL : d_item->data;
}


struct CacheItem* CacheFind2(const struct Cache* cache, const char* key, struct jaDictionaryItem** out_item)
{
	*out_item = jaDictionaryGet(cache->d, key);
	return (*out_item == NULL) ? NULL : (*out_item)->data;
}


void CacheMark(struct CacheItem* item)
{
	item->marked = true;
}


void CacheUnmark(struct CacheItem* item)
{
	(void)item;
	// TODO
}


int CacheTest()
{
	struct CacheItem* i1 = NULL;
	struct CacheItem* i2 = NULL;
	struct CacheItem* i4 = NULL;

	struct Cache* c = CacheCreate(4);

	assert(((i1 = CacheAdd(c, "spr1", 1, NULL)) != NULL)); // Success
	assert(((i2 = CacheAdd(c, "spr2", 1, NULL)) != NULL)); // Success
	assert((CacheAdd(c, "spr3", 1, NULL) != NULL));        // Success
	assert(((i4 = CacheAdd(c, "spr4", 1, NULL)) != NULL)); // Success
	assert((CacheAdd(c, "spr5", 1, NULL) == NULL));        // Failure, limit of 4 reached
	assert((CacheAdd(c, "spr4", 1, NULL) == NULL));        // Failure, already in cache

	CacheMark(i4);
	assert((CacheAdd(c, "spr4", 1, NULL) != NULL)); // Success, old one removed

	CacheMark(i2);
	CacheMark(i1);
	assert((CacheAdd(c, "spr5", 1, NULL) != NULL)); // Success, i2 is removed

	CacheDelete(c);

	return EXIT_SUCCESS;
}
