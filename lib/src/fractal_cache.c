/*
 *  fractal_cache.c -- part of FractalNow
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

#include "fractal_cache.h"
#include "fractal.h"
#include "misc.h"
#include "rectangle.h"
#include "thread.h"

#define HandleRequests(max_counter) \
if (counter == max_counter) {\
	HandlePauseRequest(threadArgHeader);\
	cancelRequested =\
		CancelTaskRequested(threadArgHeader);\
	counter = 0;\
} else {\
	++counter;\
}

int CreateFractalCache(FractalCache *cache, uint_least64_t size)
{
	int res = 0;
	cache->firstUse = 1;
	cache->fractal = (Fractal *)safeMalloc("fractal", sizeof(Fractal));
	cache->render = (RenderingParameters *)safeMalloc("rendering parameters",
							sizeof(RenderingParameters));
	cache->currentState = 0;
	cache->nbInitialized = 0;
	cache->size = size;
	cache->entry = (CacheEntry *)malloc(size*sizeof(CacheEntry));
	if (cache->entry == NULL) {
		res = 1;
		cache->size = 0;
	}
	cache->currentIndex = 0;
	cache->arrayWidth = 0;
	cache->arrayHeight = 0;
	cache->array = NULL;
	safePThreadSpinInit(&cache->arrayMutex, SPIN_INIT_ATTR);
	safePThreadSpinInit(&cache->entryMutex, SPIN_INIT_ATTR);

	return res;
}

int ResizeCacheThreadSafe(FractalCache *cache, uint_least64_t size)
{
	if (size == cache->size) {
		return 0;
	}

	int res = 1;

	safePThreadSpinLock(&cache->entryMutex);
	void *newEntry = (CacheEntry *)realloc(cache->entry, size*sizeof(CacheEntry));
	if (size == 0 || newEntry != NULL) {
		res = 0;
		cache->entry = newEntry;
		cache->size = size;
		if (cache->nbInitialized > size) {
			cache->nbInitialized = size;
		}
		if (cache->currentIndex >= size) {
			cache->currentIndex = 0;
		}
	}
	safePThreadSpinUnlock(&cache->entryMutex);

	return res;
}

inline void AddToCache(FractalCache *cache, FLOATT x, FLOATT y, FLOATT value)
{
	if (cache->size == 0) {
		return;
	}

	CacheEntry entry;
	entry.x = x;
	entry.y = y;
	entry.value = value;

	cache->entry[cache->currentIndex++] = entry;
	if (cache->currentIndex >= cache->size) {
		cache->currentIndex = 0;
	}
	if (cache->nbInitialized <= cache->size) {
		++cache->nbInitialized;
	}
}

void AddToCacheThreadSafe(FractalCache *cache, FLOATT x, FLOATT y, FLOATT value)
{
	safePThreadSpinLock(&cache->entryMutex);
	AddToCache(cache, x, y, value);
	safePThreadSpinUnlock(&cache->entryMutex);
}

static inline CacheEntry GetCacheEntry(FractalCache *cache, uint_least64_t index)
{
	return cache->entry[index];
}

static inline CacheEntry GetCacheEntryThreadSafe(FractalCache *cache, uint_least64_t index)
{
	CacheEntry res;
	safePThreadSpinLock(&cache->entryMutex);
	res = GetCacheEntry(cache, index);
	safePThreadSpinUnlock(&cache->entryMutex);
	return res;
}

typedef struct ClearCacheArrayArguments {
	uint_fast32_t threadId;
	FractalCache *cache;
	uint_fast32_t nbRectangles;
	Rectangle *rectangles;
} ClearCacheArrayArguments;

void FreeClearCacheArrayArguments(void *arg)
{
	ClearCacheArrayArguments *c_arg = (ClearCacheArrayArguments *)arg;
	if (c_arg->threadId == 0) {
		free(c_arg->rectangles);
	}
}

void *ClearCacheArrayThreadRoutine(void *arg)
{
	ThreadArgHeader *threadArgHeader = GetThreadArgHeader(arg);
	ClearCacheArrayArguments *c_arg =
		(ClearCacheArrayArguments *)GetThreadArgBody(arg);

	FractalCache *cache = c_arg->cache;
	uint_fast32_t nbRectangles = c_arg->nbRectangles;
	Rectangle *currentRect;
	uint_fast32_t rectHeight;
	ArrayValue *aVal;
	uint_fast32_t counter = 0;
	for (uint_fast32_t i = 0; i < nbRectangles; ++i) {
		currentRect = &c_arg->rectangles[i];
		rectHeight = currentRect->y2+1-currentRect->y1;
		for (uint_fast32_t j = currentRect->y1; j <= currentRect->y2; ++j) {
			SetThreadProgress(threadArgHeader, 100 * (i*rectHeight+j) /
						(nbRectangles*rectHeight));
			/* Handling pause & cancelation requests after each
			 * scanline should be good enough since operations
			 * on each pixel are trivial
			 */
			for (uint_fast32_t k = currentRect->x1; k <= currentRect->x2; ++k) {
				if (counter == 256) {
					HandlePauseRequest(threadArgHeader);
					counter = 0;
				} else {
					++counter;
				}
				aVal = &cache->array[j][k];
				aVal->state = cache->currentState;
				aVal->totalWeight = 0;
				aVal->r = 0;
				aVal->g = 0;
				aVal->b = 0;
			}

		}
	}
	SetThreadProgress(threadArgHeader, 100);

	int canceled = CancelTaskRequested(threadArgHeader);

	return (canceled ? PTHREAD_CANCELED : NULL);
}

char clearCacheArrayMessage[] = "Clearing cache array";

Task *CreateClearCacheArrayTask(FractalCache *cache, uint_fast32_t width, uint_fast32_t height,
				uint_fast32_t nbThreads)
{
	if (width == 0 || height == 0) {
		return DoNothingTask();
	}

	uint_fast32_t nbPixels = width*height;
	uint_fast32_t nbThreadsNeeded = nbThreads;
	uint_fast32_t rectanglesPerThread = DEFAULT_RECTANGLES_PER_THREAD;
	if (nbPixels <= nbThreadsNeeded) {
		nbThreadsNeeded = nbPixels;
		rectanglesPerThread = 1;
	} else if (nbPixels < nbThreadsNeeded*rectanglesPerThread) {
		rectanglesPerThread = nbPixels / nbThreadsNeeded;
	}
	uint_fast32_t nbRectangles = nbThreadsNeeded*rectanglesPerThread;

	Rectangle *rectangle;
	rectangle = (Rectangle *)safeMalloc("rectangles", nbRectangles * sizeof(Rectangle));
	InitRectangle(&rectangle[0], 0, 0, width-1, height-1);
	if (CutRectangleInN(rectangle[0], nbRectangles, rectangle)) {
		FractalNow_error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, nbRectangles);
	}
	
	ClearCacheArrayArguments *arg;
	arg = (ClearCacheArrayArguments *)safeMalloc("arguments", nbThreadsNeeded *
							sizeof(ClearCacheArrayArguments));

	for (uint_fast32_t i = 0; i < nbThreadsNeeded; ++i) {
		arg[i].threadId = i;
		arg[i].cache = cache;
		arg[i].nbRectangles = rectanglesPerThread;
		arg[i].rectangles = &rectangle[i*rectanglesPerThread];
	}
	Task *task = CreateTask(clearCacheArrayMessage, nbThreadsNeeded, arg, 
				sizeof(ClearCacheArrayArguments),
				ClearCacheArrayThreadRoutine, FreeClearCacheArrayArguments);

	free(arg);

	return task;
}

void ClearCacheArray(FractalCache *cache, Threads *threads)
{
	Task *task = CreateClearCacheArrayTask(cache, cache->arrayWidth, cache->arrayHeight,
						threads->N);
	int unused = ExecuteTaskBlocking(task, threads);
	UNUSED(unused);
}

static inline Color ColorFromValue(FLOATT value, const RenderingParameters *render)
{
	Color res;
	if (value < 0) {
		res = render->spaceColor;
	} else {
		value = render->transferFunctionPtr(value)*render->realMultiplier+
			render->realOffset;
		res = GetGradientColor(&render->gradient, (uint_fast64_t)(value));
	}

	return res;
}

static inline ArrayValue aux_PutIntoArray(FractalCache *cache,
						uint_fast32_t x, uint_fast32_t y,
						FLOATT r, FLOATT g, FLOATT b,
						FLOATT weight)
{
	ArrayValue *aVal = &cache->array[y][x];
	if (aVal->state != cache->currentState) {
		aVal->r = 0;
		aVal->g = 0;
		aVal->b = 0;
		aVal->totalWeight = 0;
		aVal->state = cache->currentState;
	}
	aVal->r += r;
	aVal->g += g;
	aVal->b += b;
	aVal->totalWeight += weight;

	return *aVal;
}

static inline ArrayValue PutIntoArray(FractalCache *cache,
					const RenderingParameters *render,
					uint_fast32_t x, uint_fast32_t y,
					FLOATT value, FLOATT weight)
{
	Color color = ColorFromValue(value, render);
	FLOATT r = color.r * weight;
	FLOATT g = color.g * weight;
	FLOATT b = color.b * weight;

	return aux_PutIntoArray(cache, x, y, r, g, b, weight);
}

static inline ArrayValue PutIntoArrayThreadSafe(FractalCache *cache,
						const RenderingParameters *render,
						uint_fast32_t x, uint_fast32_t y,
						FLOATT value, FLOATT weight)
{
	Color color = ColorFromValue(value, render);
	FLOATT r = color.r * weight;
	FLOATT g = color.g * weight;
	FLOATT b = color.b * weight;
	safePThreadSpinLock(&cache->arrayMutex);
	ArrayValue res = aux_PutIntoArray(cache, x, y, r, g, b, weight);
	safePThreadSpinUnlock(&cache->arrayMutex);

	return res;
}

inline ArrayValue GetArrayValue(FractalCache *cache, uint_fast32_t x, uint_fast32_t y)
{
	return cache->array[y][x];
}

static inline ArrayValue GetArrayValueThreadSafe(FractalCache *cache, uint_fast32_t x,
							uint_fast32_t y)
{
	safePThreadSpinLock(&cache->arrayMutex);
	ArrayValue aVal = GetArrayValue(cache, x, y);
	safePThreadSpinUnlock(&cache->arrayMutex);

	return aVal;
}

inline int isArrayValueValid(ArrayValue aVal, FractalCache *cache)
{
	return (aVal.state == cache->currentState &&
		aVal.totalWeight > DEFAULT_CACHE_WEIGHT_THRESHOLD);
}

inline Color GetColorFromAVal(ArrayValue aVal, const RenderingParameters *render)
{
	Color res;

	res.bytesPerComponent = render->bytesPerComponent;
	res.r = aVal.r / aVal.totalWeight;
	res.g = aVal.g / aVal.totalWeight;
	res.b = aVal.b / aVal.totalWeight;

	return res;
}

static inline int isInsideArray(FLOATT x, FLOATT y, uint_fast32_t width, uint_fast32_t height)
{
	return (x >= 0 && x <= width-1 && y <= height-1 && y >= 0);
}

typedef struct ResizeCacheArrayArguments {
	FractalCache *cache;
	uint_fast32_t width;
	uint_fast32_t height;
} ResizeCacheArrayArguments;

void FreeResizeCacheArrayArguments(void *arg)
{
	UNUSED(arg);
}

void ResizeCacheArray(FractalCache *cache, uint_fast32_t width, uint_fast32_t height)
{
	for (uint_fast32_t i = 0; i < cache->arrayHeight; ++i) {
		free(cache->array[i]);
	}
	cache->array = (ArrayValue **)safeRealloc("cache array", cache->array,
						height*sizeof(ArrayValue *));
	for (uint_fast32_t i = 0; i < height; ++i) {
		cache->array[i] = (ArrayValue *)safeMalloc("cache array",
						width*sizeof(ArrayValue));
	}

	cache->arrayWidth = width;
	cache->arrayHeight = height;
}

void *ResizeCacheArrayThreadRoutine(void *arg)
{
	ResizeCacheArrayArguments *c_arg =
		(ResizeCacheArrayArguments *)GetThreadArgBody(arg);

	ResizeCacheArray(c_arg->cache, c_arg->width, c_arg->height);

	return NULL;
}

char resizeCacheArrayMessage[] = "Resizing cache array";

Task *CreateResizeCacheArrayTask(FractalCache *cache, uint_fast32_t width,
					uint_fast32_t height)
{
	ResizeCacheArrayArguments arg;

	arg.cache = cache;
	arg.width = width;
	arg.height = height;

	Task *task = CreateTask(resizeCacheArrayMessage, 1, &arg, 
				sizeof(ResizeCacheArrayArguments),
				ResizeCacheArrayThreadRoutine,
				FreeResizeCacheArrayArguments);

	return task;
}

int InvalidateCacheArray(FractalCache *cache)
{
	int res = 0;
	if (cache->currentState == UINT_FAST32_MAX) {
		cache->currentState = 0;
		res = 1;
	} else {
		++cache->currentState;
	}

	return res;
}

typedef struct FillCacheArrayArguments {
	uint_fast32_t threadId;
	uint_least64_t begin, end;
	Image *image;
	pthread_spinlock_t *imageMutex;
	const Fractal *fractal;
	const RenderingParameters *render;
	FractalCache *cache;
} FillCacheArrayArguments;

void FreeFillCacheArrayArguments(void *arg)
{
	FillCacheArrayArguments *c_arg = (FillCacheArrayArguments *)arg;

	if (c_arg->threadId == 0) {
		safePThreadSpinDestroy(c_arg->imageMutex);
		free((void *)c_arg->imageMutex);
	}
}

void *FillCacheArrayThreadRoutine(void *arg)
{
	ThreadArgHeader *threadArgHeader = GetThreadArgHeader(arg);
	FillCacheArrayArguments *c_arg =
		(FillCacheArrayArguments *)GetThreadArgBody(arg);

	FractalCache *cache = c_arg->cache;
	const Fractal *fractal = c_arg->fractal;
	const RenderingParameters *render = c_arg->render;
	Image *image = c_arg->image;
	pthread_spinlock_t *imageMutex = c_arg->imageMutex;
	CacheEntry entry;
	FLOATT x, y;
	uint_least64_t nbEntries = c_arg->end-c_arg->begin+1;

	uint_fast32_t counter = 0;
	int cancelRequested = CancelTaskRequested(threadArgHeader);
	FLOATT dx, dy;
	uint_fast32_t intX, intY;
	FLOATT sigma = 1./3;
	FLOATT sigma2_x_2 = sigma*sigma*2;
	FLOATT weight;
	ArrayValue aVal;
	Color color;
	for (uint_least64_t i = c_arg->begin; i <= c_arg->end && !cancelRequested; ++i) {
		SetThreadProgress(threadArgHeader, 100 * (i-c_arg->begin) / nbEntries);
		HandleRequests(128);
		entry = GetCacheEntry(cache, i);

		x = (entry.x - fractal->x1) * cache->arrayWidth / fractal->spanX-0.5;
		y = (entry.y - fractal->y1) * cache->arrayHeight / fractal->spanY-0.5;

		intX = roundF(x);
		intY = roundF(y);
		dx = x - intX;
		dy = y - intY;
		if (isInsideArray(intX, intY, cache->arrayWidth, cache->arrayHeight)) {

			weight = expF(-(dx*dx+dy*dy)/sigma2_x_2);
			if (image != NULL) {
				safePThreadSpinLock(imageMutex);
				aVal = PutIntoArray(cache, render, intX, intY,
							entry.value, weight);
				color = GetColorFromAVal(aVal, render);

				PutPixelUnsafe(image, intX, intY, color);
				safePThreadSpinUnlock(imageMutex);
			} else {
				aVal = PutIntoArrayThreadSafe(cache, render, intX, intY,
								entry.value, weight);
			}
		}
	}
	SetThreadProgress(threadArgHeader, 100);

	int canceled = CancelTaskRequested(threadArgHeader);

	return (canceled ? PTHREAD_CANCELED : NULL);
}

char fillCacheArrayMessage[] = "Filling cache array";

Task *CreateFillCacheArrayTask(FractalCache *cache, Image *image, const Fractal *fractal,
				const RenderingParameters *render,
				uint_fast32_t nbThreads)
{
	if (cache->size == 0 || cache->arrayWidth == 0 || cache->arrayHeight == 0
			|| cache->nbInitialized == 0) {
		return DoNothingTask();
	}

	uint_fast32_t nbEntries = cache->nbInitialized;
	uint_fast32_t nbThreadsNeeded = nbThreads;
	if (nbEntries <= nbThreadsNeeded) {
		nbThreadsNeeded = nbEntries;
	}
	uint_fast32_t nbRectangles = nbThreadsNeeded;

	Rectangle *rectangle;
	rectangle = (Rectangle *)safeMalloc("rectangles", nbRectangles * sizeof(Rectangle));
	InitRectangle(&rectangle[0], 0, 0, nbEntries-1, 0);
	if (CutRectangleInN(rectangle[0], nbRectangles, rectangle)) {
		FractalNow_error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, nbRectangles);
	}
	
	pthread_spinlock_t *imageMutex;
	imageMutex = (pthread_spinlock_t *)safeMalloc("image mutex",
							sizeof(pthread_spinlock_t));
	safePThreadSpinInit(imageMutex, SPIN_INIT_ATTR);

	FillCacheArrayArguments *arg;
	arg = (FillCacheArrayArguments *)safeMalloc("arguments", nbThreadsNeeded *
							sizeof(FillCacheArrayArguments));

	for (uint_fast32_t i = 0; i < nbThreadsNeeded; ++i) {
		arg[i].threadId = i;
		arg[i].cache = cache;
		arg[i].image = image;
		arg[i].imageMutex = imageMutex;
		arg[i].fractal = fractal;
		arg[i].render = render;
		arg[i].begin = rectangle[i].x1;
		arg[i].end = rectangle[i].x2;
	}
	Task *task = CreateTask(fillCacheArrayMessage, nbThreadsNeeded, arg, 
				sizeof(FillCacheArrayArguments),
				FillCacheArrayThreadRoutine, FreeFillCacheArrayArguments);

	free(arg);
	free(rectangle);

	return task;
}

void FillCacheArray(FractalCache *cache, Image *image, const Fractal *fractal,
			const RenderingParameters *render, Threads *threads)
{
	Task *task = CreateFillCacheArrayTask(cache, image, fractal, render, threads->N);
	int unused = ExecuteTaskBlocking(task, threads);
	UNUSED(unused);
}

typedef struct FillImageFromCacheArrayArguments {
	uint_fast32_t threadId;
	FractalCache *cache;
	Image *image;
	const RenderingParameters *render;
	uint_fast32_t nbRectangles;
	Rectangle *rectangles;
} FillImageFromCacheArrayArguments;

void FreeFillImageFromCacheArrayArguments(void *arg)
{
	FillImageFromCacheArrayArguments *c_arg =
		(FillImageFromCacheArrayArguments *)arg;
	if (c_arg->threadId == 0) {
		free(c_arg->rectangles);
	}
}

void *FillImageFromCacheArrayThreadRoutine(void *arg)
{
	ThreadArgHeader *threadArgHeader = GetThreadArgHeader(arg);
	FillImageFromCacheArrayArguments *c_arg =
		(FillImageFromCacheArrayArguments *)GetThreadArgBody(arg);

	FractalCache *cache = c_arg->cache;
	Image *dst = c_arg->image;
	const RenderingParameters *render = c_arg->render;

	uint_fast32_t nbRectangles = c_arg->nbRectangles;
	Rectangle *currentRect;
	uint_fast32_t rectHeight;
	ArrayValue aVal;
	uint_fast32_t counter = 0;
	int cancelRequested = CancelTaskRequested(threadArgHeader);
	Color color;
	for (uint_fast32_t i = 0; i < nbRectangles && !cancelRequested; ++i) {
		currentRect = &c_arg->rectangles[i];
		rectHeight = currentRect->y2+1-currentRect->y1;
		for (uint_fast32_t j = currentRect->y1; j <= currentRect->y2 
				&& !cancelRequested; ++j) {
			SetThreadProgress(threadArgHeader, 100 * (i*rectHeight+j) /
						(nbRectangles*rectHeight));
			for (uint_fast32_t k = currentRect->x1; k <= currentRect->x2
					&& !cancelRequested; ++k) {
				HandleRequests(128);

				aVal = GetArrayValue(cache, k, j);
				if (isArrayValueValid(aVal, cache)) {
					color = GetColorFromAVal(aVal, render);
					PutPixelUnsafe(dst, k, j, color);
				}
			}
		}
	}
	SetThreadProgress(threadArgHeader, 100);

	int canceled = CancelTaskRequested(threadArgHeader);

	return (canceled ? PTHREAD_CANCELED : NULL);
} 

char fillImageFromCacheArrayMessage[] = "Filling image from cache array";

Task *CreateFillImageFromCacheArrayTask(Image *dst, FractalCache *cache,
					const RenderingParameters *render,
					uint_fast32_t nbThreads)
{
	if (dst->width == 0 || dst->height == 0) {
		return DoNothingTask();
	}

	uint_fast32_t nbPixels = dst->width*dst->height;
	uint_fast32_t nbThreadsNeeded = nbThreads;
	uint_fast32_t rectanglesPerThread = DEFAULT_RECTANGLES_PER_THREAD;
	if (nbPixels <= nbThreadsNeeded) {
		nbThreadsNeeded = nbPixels;
		rectanglesPerThread = 1;
	} else if (nbPixels < nbThreadsNeeded*rectanglesPerThread) {
		rectanglesPerThread = nbPixels / nbThreadsNeeded;
	}
	uint_fast32_t nbRectangles = nbThreadsNeeded*rectanglesPerThread;

	Rectangle *rectangle;
	rectangle = (Rectangle *)safeMalloc("rectangles", nbRectangles * sizeof(Rectangle));
	InitRectangle(&rectangle[0], 0, 0, dst->width-1, dst->height-1);
	if (CutRectangleInN(rectangle[0], nbRectangles, rectangle)) {
		FractalNow_error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, nbRectangles);
	}
	
	FillImageFromCacheArrayArguments *arg;
	arg = (FillImageFromCacheArrayArguments *)safeMalloc("arguments", nbThreadsNeeded *
						sizeof(FillImageFromCacheArrayArguments));

	for (uint_fast32_t i = 0; i < nbThreadsNeeded; ++i) {
		arg[i].threadId = i;
		arg[i].cache = cache;
		arg[i].image = dst;
		arg[i].render = render;
		arg[i].nbRectangles = rectanglesPerThread;
		arg[i].rectangles = &rectangle[i*rectanglesPerThread];
	}
	Task *task = CreateTask(fillImageFromCacheArrayMessage, nbThreadsNeeded, arg, 
				sizeof(FillImageFromCacheArrayArguments),
				FillImageFromCacheArrayThreadRoutine,
				FreeFillImageFromCacheArrayArguments);

	free(arg);

	return task;
}

void FillImageFromCacheArray(Image *dst, FractalCache *cache, 
				const RenderingParameters *render, Threads *threads)
{
	Task *task = CreateFillImageFromCacheArrayTask(dst, cache, render,
			threads->N);
	int unused = ExecuteTaskBlocking(task, threads);
	UNUSED(unused);
}

Task *CreateFillArrayAndImageFromCacheTask(FractalCache *cache, Image *image,
					const Fractal *fractal,
					const RenderingParameters *render,
					int fillImageOnTheFly, uint_fast32_t nbThreads)
{
	Task *res;
	if (fillImageOnTheFly) {
		res = CreateFillCacheArrayTask(cache, image, fractal, render, nbThreads);
	} else {
		Task *subTasks[2];
		subTasks[0] = CreateFillCacheArrayTask(cache, NULL, fractal, render,	
							nbThreads);
		subTasks[1] = CreateFillImageFromCacheArrayTask(image, cache, render,
								nbThreads);
		res = CreateCompositeTask(NULL, 2, subTasks);
	}

	return res;
}

void FillArrayAndImageFromCache(FractalCache *cache, Image *image,
				const Fractal *fractal,
				const RenderingParameters *render,
				int fillImageOnTheFly, Threads *threads)
{
	Task *task = CreateFillArrayAndImageFromCacheTask(cache, image, fractal,
							render, fillImageOnTheFly,
							threads->N);
	int unused = ExecuteTaskBlocking(task, threads);
	UNUSED(unused);
}

void aux_CreateFractalCachePreviewTask(FractalCache *cache, const Fractal *fractal,
					const RenderingParameters *render)
{
	safePThreadSpinLock(&cache->entryMutex);
	cache->nbInitialized = 0;
	cache->currentIndex = 0;
	safePThreadSpinUnlock(&cache->entryMutex);
	if (cache->firstUse) {
		cache->firstUse = 0;
	} else {
		FreeFractal(*cache->fractal);
		FreeRenderingParameters(*cache->render);
	}
	*cache->fractal = CopyFractal(fractal);
	*cache->render = CopyRenderingParameters(render);
}

enum TaskType {
	DO_NOTHING=0,
	RESIZE_CLEAR,
	RESIZE_CLEAR_FILL,
	CLEAR_FILL,
	FILL
};

static inline int PartCompareFractals(const Fractal *fractal1, const Fractal *fractal2)
{
	return (fractal1->fractalFormula != fractal2->fractalFormula ||
		fractal1->p != fractal2->p ||
		fractal1->c != fractal2->c ||
		fractal1->escapeRadius != fractal2->escapeRadius ||
		fractal1->maxIter != fractal2->maxIter);
}

static inline int PartCompareRenderingParameters(const RenderingParameters *param1,
							const RenderingParameters *param2)
{
	return (param1->bytesPerComponent != param2->bytesPerComponent ||
		CompareColors(param1->spaceColor, param2->spaceColor) ||
		param1->countingFunction != param2->countingFunction ||
		param1->coloringMethod != param2->coloringMethod ||
		param1->addendFunction != param2->addendFunction ||
		param1->stripeDensity != param2->stripeDensity ||
		param1->interpolationMethod != param2->interpolationMethod);
		
}

char fractalCachePreviewMessage[] = "Creating fractal preview from cache";

Task *CreateFractalCachePreviewTask(Image *dst, FractalCache *cache, const Fractal *fractal,
					const RenderingParameters *render,
					int fillImageOnTheFly, uint_fast32_t nbThreads)
{
	Task *res;
	Fractal *cacheFractal = cache->fractal;
	RenderingParameters *cacheRender = cache->render;
	enum TaskType taskType;
	if (cache->firstUse) {
		aux_CreateFractalCachePreviewTask(cache, fractal, render);
		taskType = RESIZE_CLEAR;
	} else if (PartCompareFractals(cacheFractal, fractal) ||
			PartCompareRenderingParameters(cacheRender, render)) {
		
		aux_CreateFractalCachePreviewTask(cache, fractal, render);

		if (dst->width != cache->arrayWidth || dst->height != cache->arrayHeight) {
			taskType = RESIZE_CLEAR;
		} else {
			int unused = InvalidateCacheArray(cache);
			UNUSED(unused);
			taskType = DO_NOTHING;
		}
	} else if (dst->width != cache->arrayWidth || dst->height != cache->arrayHeight) {
		taskType = RESIZE_CLEAR_FILL;
	} else {
		if (InvalidateCacheArray(cache)) {
			taskType = CLEAR_FILL;
		} else {
			taskType = FILL;
		}
	}

	switch (taskType) {
	case DO_NOTHING: {
		Task *subTask = DoNothingTask();
		res = CreateCompositeTask(fractalCachePreviewMessage, 1, &subTask);
		break;
		}
	case RESIZE_CLEAR: {
		Task *subTasks[2];
		subTasks[0] = CreateResizeCacheArrayTask(cache, dst->width, dst->height);
		subTasks[1] = CreateClearCacheArrayTask(cache, dst->width, dst->height,
							nbThreads);
		res = CreateCompositeTask(fractalCachePreviewMessage, 2, subTasks);
		break;
		}
	case RESIZE_CLEAR_FILL: {
		Task *subTasks[3];
		subTasks[0] = CreateResizeCacheArrayTask(cache, dst->width, dst->height);
		subTasks[1] = CreateClearCacheArrayTask(cache, dst->width, dst->height,
							nbThreads);
		subTasks[2] = CreateFillArrayAndImageFromCacheTask(cache, dst, fractal, render,
								fillImageOnTheFly, nbThreads);
		res = CreateCompositeTask(fractalCachePreviewMessage, 3, subTasks);
		break;
		}
	case CLEAR_FILL: {
		Task *subTasks[2];
		subTasks[0] = CreateClearCacheArrayTask(cache, dst->width,
							dst->height, nbThreads);
		subTasks[1] = CreateFillArrayAndImageFromCacheTask(cache, dst, fractal, render,
								fillImageOnTheFly, nbThreads);
		res = CreateCompositeTask(fractalCachePreviewMessage, 2, subTasks);
		break;
		}
	case FILL: {
		Task *subTask =
			CreateFillArrayAndImageFromCacheTask(cache, dst, fractal, render,
								fillImageOnTheFly, nbThreads);
		res = CreateCompositeTask(fractalCachePreviewMessage, 1, &subTask);
		}
		break;
	}

	return res;
}

void FractalCachePreview(Image *dst, FractalCache *cache, const Fractal *fractal,
				const RenderingParameters *render, int fillImageOnTheFly,
				Threads *threads)
{
	Task *task = CreateFractalCachePreviewTask(dst, cache, fractal, render,
							fillImageOnTheFly, threads->N);
	int unused = ExecuteTaskBlocking(task, threads);
	UNUSED(unused);
}

void FreeFractalCache(FractalCache *cache)
{
	free(cache->entry);
	for (uint_fast32_t i = 0; i < cache->arrayHeight; ++i) {
		free(cache->array[i]);
	}
	free(cache->array);
	if (!cache->firstUse) {
		FreeFractal(*cache->fractal);
		FreeRenderingParameters(*cache->render);
	}
	free(cache->fractal);
	free(cache->render);
	safePThreadSpinDestroy(&cache->entryMutex);
	safePThreadSpinDestroy(&cache->arrayMutex);
}
