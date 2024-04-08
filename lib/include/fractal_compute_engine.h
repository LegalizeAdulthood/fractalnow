/*
 *  fractal_compute_engine.h -- part of FractalNow
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
  * \file fractal_compute_engine.h
  * \brief Header file related to fractal computation engines.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_COMPUTE_ENGINE_H__
#define __FRACTAL_COMPUTE_ENGINE_H__

#include "float_precision.h"
#include "fractal_addend_function.h"
#include "fractal_cache.h"
#include "fractal_coloring.h"
#include "fractal_formula.h"
#include "fractal_iteration_count.h"
#include "fractal_rendering_parameters.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Fractal;

/**
 * \struct FractalEngine
 * \brief Engine to compute fractal engine (specific compiled loop and data).
 *
 * A FractalEngine structure contains a pointer to a function that computes
 * fractal for given parameters efficiently, as well as all the data it needs.\n
 *
 * The cache entry returned by fractal loop can be added to a cache, in which
 * case it will be free'd automatically (when new entries come, or when cache
 * is free'd), or just free'd if cache is disabled.
 */
/**
 * \typedef FractalEngine
 * \brief Convenient typedef for struct FractalEngine.
 */
typedef struct FractalEngine {
	CacheEntry (*fractalLoop)(void *data, const struct Fractal *fractal,
				const RenderingParameters *render,
				uint_fast32_t x, uint_fast32_t y,
				uint_fast32_t width, uint_fast32_t height);
	/*!< Fractal loop function.*/
	void (*freeEngineData)(void *data);
	/*!< Function to free engine data.*/
	void *data;
	/*!< Engine data (used by fractal loop).*/
} FractalEngine;

/**
 * \fn void CreateFractalEngine(FractalEngine *engine, const struct Fractal *fractal, const RenderingParameters *render, FloatPrecision floatPrecision)
 * \brief Create fractal engine for given fractal, rendering parameters and float precision.
 *
 * \param engine Pointer to structure to be initialized.
 * \param fractal Fractal to be computed.
 * \param render Rendenring parameters.
 * \param floatPrecision Float precision.
 */
void CreateFractalEngine(FractalEngine *engine, const struct Fractal *fractal,
				const RenderingParameters *render, FloatPrecision floatPrecision);

/**
 * \fn void FreeFractalEngine(FractalEngine *engine)
 * \brief Free fractal engine.
 *
 * \param engine Pointer to fractal structure to be free'd.
 */
void FreeFractalEngine(FractalEngine *engine);

/**
 * \fn CacheEntry RunFractalEngine(const FractalEngine *engine, const struct Fractal *fractal, const RenderingParameters *render, uint_fast32_t x, uint_fast32_t y, uint_fast32_t width, uint_fast32_t height)
 * \brief Run fractal engine at given point.
 *
 * Compute pixel (x,y) of an image of size (width, height).
 * \see CacheEntry
 *
 * \param engine Fractal engine to be run.
 * \param fractal Fractal to be computed.
 * \param render Rendering parameters.
 * \param x Pixel X coordinate.
 * \param y Pixel Y coordinate.
 * \param width Image width.
 * \param height Image height.
 * \return Cache entry that can be added to cache or just used and free'd.
 */
CacheEntry RunFractalEngine(const FractalEngine *engine, const struct Fractal *fractal,
			const RenderingParameters *render, uint_fast32_t x, uint_fast32_t y,
			uint_fast32_t width, uint_fast32_t height);

#ifdef __cplusplus
}
#endif

#endif
