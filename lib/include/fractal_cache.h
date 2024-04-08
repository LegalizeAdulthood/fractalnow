/*
 *  fractal_cache.h -- part of FractalNow
 *
 *  Copyright (c) 2012 Marc Pegon <pe.marc@free.fr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

 /**
  * \file fractal_cache.h
  * \brief Main header file related to fractal cache.
  */

#ifndef __FRACTAL_CACHE_H__
#define __FRACTAL_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <stdint.h>
#include "color.h"
#include "fractal_rendering_parameters.h"
#include "floating_point.h"
#include "image.h"
#include "thread.h"

/**
 * \def DEFAULT_FRACTAL_CACHE_SIZE
 * \brief Default fractal cache size (number of entries).
 */
#define DEFAULT_FRACTAL_CACHE_SIZE (uint_least64_t)(300000)

/**
 * \def DEFAULT_CACHE_WEIGHT_THRESHOLD
 * \brief Default threshold for ArrayValue validity.
 *
 * \see isArrayValueValid
 */
#define DEFAULT_CACHE_WEIGHT_THRESHOLD (FLOATT)(0.)

/**
 * \struct CacheEntry
 * \brief Cache entry.
 */
/**
 * \typedef CacheEntry
 * \brief Convenient typedef for struct CacheEntry.
 */
typedef struct CacheEntry {
	FLOATT x;
 /*!< x coordinate.*/
	FLOATT y;
 /*!< y coordinate.*/
	FLOATT value;
 /*!< Value at point (x,y).*/
} CacheEntry;

/**
 * \struct ArrayValue
 * \brief Type of cache array elements.
 */
/**
 * \typedef ArrayValue
 * \brief Convenient typedef for struct ArrayValue.
 */
typedef struct ArrayValue {
	uint_fast32_t state;
 /*!< Value state, used to test validity.*/
	FLOATT r;
 /*!< Sum of weighted red values.*/
	FLOATT g;
 /*!< Sum of weighted green values.*/
	FLOATT b;
 /*!< Sum of weighted blue values.*/
	FLOATT totalWeight;
 /*!< Total weight.*/
} ArrayValue;

struct Fractal;

/**
 * \struct FractalCache
 * \brief Fractal cache for fractal drawing.
 *
 * When using cache to create an image preview
 * all entries are placed at the correct position
 * in the array (resized to image size), and the
 * image is produced from this array.
 */
/**
 * \typedef FractalCache
 * \brief Convenient type for struct FractalCache.
 */
typedef struct FractalCache {
	int firstUse;
 /*!< Indicates whether or not cache has already been used.*/
	struct Fractal *fractal;
 /*!< Current fractal cached.*/
	RenderingParameters *render;
 /*!< Current rendering parameters cached.*/
	uint_fast32_t currentState;
 /*!< Current cache state (used for testing array values validity.*/
	uint_least64_t nbInitialized;
 /*!< Number of entries already initialized (if cache is not full yet).*/
	uint_least64_t size;
 /*!< Cache size (total number of entries it can contain).*/
	CacheEntry *entry;
 /*!< Array of entries.*/
	uint_least64_t currentIndex;
 /*!< Current entry index.*/
	uint_fast32_t arrayWidth;
 /*!< Array width.*/
	uint_fast32_t arrayHeight;
 /*!< Array height.*/
	ArrayValue **array;
 /*!< Cache array.*/
	pthread_spinlock_t arrayMutex;
 /*!< Mutex for cache array.*/
	pthread_spinlock_t entryMutex;
 /*!< Mutex for cache entries.*/
} FractalCache;

/**
 * \fn int CreateFractalCache(FractalCache *cache, uint_least64_t size)
 * \brief Create fractal cache.
 *
 * In case of failure, cache is initialised with size = 0.
 *
 * \param cache Pointer to cache structure to initialize.
 * \param size Cache size (maximum number of entries).
 * \return 0 in case of success, 1 otherwise.
 */
int CreateFractalCache(FractalCache *cache, uint_least64_t size);

/**
 * \fn int ResizeCacheThreadSafe(FractalCache *cache, uint_least64_t size)
 * \brief Resize cache (thread-safe).
 *
 * In case of failure, cache is left unchanged.
 *
 * \param cache Pointer to cache to resize.
 * \param size New cache size (maximum number of entries).
 * \return 0 in case of success, 1 otherwise.
 */
int ResizeCacheThreadSafe(FractalCache *cache, uint_least64_t size);

/**
 * \fn void AddToCache(FractalCache *cache, FLOATT x, FLOATT y, FLOATT value)
 * \brief Add entry to cache.
 *
 * This function is not thread-safe (like most of the
 * functions in this library).\n
 * \see AddToCacheThreadSafe
 *
 * \param cache Pointer to cache.
 * \param x X coordinate.
 * \param y Y coordinate.
 * \param value Value at point (x,y).
 */
void AddToCache(FractalCache *cache, FLOATT x, FLOATT y, FLOATT value);

/**
 * \fn void AddToCacheThreadSafe(FractalCache *cache, FLOATT x, FLOATT y, FLOATT value)
 * \brief Add entry to cache (thread-safe).
 *
 * This function is thread-safe (unlike most of the
 * functions in this library).\n
 * \see AddToCache
 *
 * \param cache Pointer to cache.
 * \param x X coordinate.
 * \param y Y coordinate.
 * \param value Value at point (x,y).
 */
void AddToCacheThreadSafe(FractalCache *cache, FLOATT x, FLOATT y, FLOATT value);

/**
 * \fn ArrayValue GetArrayValue(FractalCache *cache, uint_fast32_t x, uint_fast32_t y)
 * \brief Get value from cache array.
 *
 * The function does not check that (x,y) is within
 * cache array range (hence unspecified behaviour
 * otherwise).
 *
 * \param cache Pointer to cache structure.
 * \param x X coordinate.
 * \param y Y coordinate.
 * \return Array value at point (x,y).
 */
ArrayValue GetArrayValue(FractalCache *cache, uint_fast32_t x, uint_fast32_t y);

/**
 * \fn int isArrayValueValid(ArrayValue aVal, FractalCache *cache)
 * \brief Check whether an array value is valid or not.
 *
 * If an array value is valid, it can be used to create
 * a preview of a fractal image.\n
 * An array value is valid if:
 *   -its state is equal to cache current state
 *   -its total weight is stricly greater than
 *    DEFAULT_CACHE_WEIGHT_THRESHOLD
 * This "cache and value state" is a trick used to avoid
 * having to clear cache array before creating an
 * image preview.
 *
 * \param aVal Array value.
 * \param cache Pointer to cache structure.
 * \return 1 of value is valid, 0 otherwise.
 */
int isArrayValueValid(ArrayValue aVal, FractalCache *cache);

/**
 * \fn Color GetColorFromAVal(ArrayValue aVal, const RenderingParameters *render)
 * \brief Get color from array value.
 *
 * Array value *must* be valid to get color from it.\n
 * \see isArrayValueValid
 *
 * \param aVal Array value.
 * \param render Rendering parameters to render color.
 * \return Corresponding color.
 */
Color GetColorFromAVal(ArrayValue aVal, const RenderingParameters *render);

/**
 * \fn Task *CreateFractalCachePreviewTask(Image *dst, FractalCache *cache, const struct Fractal *fractal, const RenderingParameters *render, int fillImageOnTheFly, uint_fast32_t nbThreads)
 * \brief Create task generating an image preview from cache.
 *
 * The task generate an image preview AND updates the
 * cache array.\n
 * When the image is not filled on the fly, image begins
 * to be filled AFTER cache array has been updated.\n
 * When the image is filled on the fly, image is filled
 * while cache array is being filled.\n
 * The "on the fly" can be used to see image being filled
 * as soon as possible, but can be costlier.
 *
 * \param dst Destination image.
 * \param cache Cache.
 * \param fractal Fractal.
 * \param render Rendering parameters.
 * \param fillImageOnTheFly Indicates if the image should be filled on the fly.
 * \param nbThreads Number of threads that action will need to be launched.
 * \return Corresponding newly-allocated task.
 */
Task *CreateFractalCachePreviewTask(Image *dst, FractalCache *cache,
					const struct Fractal *fractal,
					const RenderingParameters *render,
					int fillImageOnTheFly,
					uint_fast32_t nbThreads);

/**
 * \fn void FractalCachePreview(Image *dst, FractalCache *cache, const struct Fractal *fractal, const RenderingParameters *render, int fillImageOnTheFly, Threads *threads)
 * \brief Generate fractal image preview from cache (blocking).
 *
 * \see CreateFractalCachePreviewTask
 *
 * \param dst Destination image.
 * \param cache Cache.
 * \param fractal Fractal.
 * \param render Rendering parameters.
 * \param fillImageOnTheFly Indicates if the image should be filled on the fly.
 * \param threads Threads to be used for task.
 */
void FractalCachePreview(Image *dst, FractalCache *cache, const struct Fractal *fractal,
				const RenderingParameters *render, int fillImageOnTheFly,
				Threads *threads);

/**
 * \fn void FreeFractalCache(FractalCache *cache)
 * \brief Free fractal cache.
 *
 * \param cache Pointer to cache structure to be free'd.
 */
void FreeFractalCache(FractalCache *cache);

#ifdef __cplusplus
}
#endif

#endif

