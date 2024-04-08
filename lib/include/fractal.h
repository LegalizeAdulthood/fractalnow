/*
 *  fractal.h -- part of FractalNow
 *
 *  Copyright (c) 2011 Marc Pegon <pe.marc@free.fr>
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
  * \file fractal.h
  * \brief Main header file related to fractals.
  *
  * A fractal point is computed and colored as follows :
  * - The fractal loop function is executed, and returns a floating point
  *   value.
  *   \see fractal_loop.h
  * - The transfer function is applied, as well as the multiplier, and the
  *   offset is added.
  * - Finally, the value is mapped to a color using the gradient.
  *
  * Note that points that belong to the fractal set (radius <= escape radius
  * after the maximum number of iterations) do not go through all those
  * steps : the fractal loop gives a negative value, so that the color of
  * fractal space is returned.
  *
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_H__
#define __FRACTAL_H__

#include "float_precision.h"
#include "complex_wrapper.h"
#include "gradient.h"
#include "image.h"
#include "fractal_cache.h"
#include "fractal_formula.h"
#include "fractal_rendering_parameters.h"
#include "task.h"
#include "thread.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \def DEFAULT_QUAD_INTERPOLATION_SIZE
 * \brief Default maximum size of quadrilaterals for interpolation.
 *
 * \see DrawFractalFast for more details.
 */
#define DEFAULT_QUAD_INTERPOLATION_SIZE (uint_fast32_t)(5)

/**
 * \def DEFAULT_COLOR_DISSIMILARITY_THRESHOLD
 * \brief Default color dissimilarity threshold for quad interpolation.
 *
 * \see DrawFractalFast for more details.
 */
#define DEFAULT_COLOR_DISSIMILARITY_THRESHOLD (double)(3.5E-3)

/**
 * \def DEFAULT_ADAPTIVE_AAM_THRESHOLD
 * \brief Default threshold for adaptive anti-aliasing.
 *
 * \see AntiAliaseFractal for more details.
 */
#define DEFAULT_ADAPTIVE_AAM_THRESHOLD (double)(5.05E-2)

/**
 * \struct Fractal
 * \brief Description of a subset of some fractal set.
 */
/**
 * \typedef Fractal
 * \brief Convenient typedef for struct Fractal.
 */
typedef struct Fractal {
	FractalFormula fractalFormula;
 /*!< Fractal formula.*/
	BiggestComplexFloat p;
 /*!< Parameter for some fractals (main power in iteration z = z^p + ...).*/
	BiggestComplexFloat c;
 /*!< Parameter for Julia and Rudy fractal.*/
	BiggestFloat centerX;
 /*!< X coordinate of the center of the fractal subset.*/
	BiggestFloat centerY;
 /*!< Y coordinate of the center of the fractal subset.*/
	BiggestFloat spanX;
 /*!< X span of the fractal subset.*/
	BiggestFloat spanY;
 /*!< Y span of the fractal subset.*/
	double escapeRadius;
 /*!< Escape radius for computing fractal.*/
	uint_fast32_t maxIter;
 /*!< Maximum number of iterations for computing fractal.*/

 /* Some parameters for internal use.*/
	BiggestFloat x1;
 /*!< X coordinate of the upper left point of the fractal subset.*/
	BiggestFloat y1;
 /*!< Y coordinate of the upper left point of the fractal subset.*/
} Fractal;

/**
 * \fn void InitFractal(Fractal *fractal, FractalFormula fractalFormula, Complex_l p, Complex_l complex c, long double centerX, long double centerY, long double spanX, long double spanY, double escapeRadius, uint_fast32_t maxIter)
 * \brief Initialize fractal structure.
 *
 * \param fractal Pointer to fractal structure to initialize.
 * \param fractalFormula Fractal type.
 * \param p (main power in iteration) parameter for fractal.
 * \param c Parameter for Julia fractal (will be ignored for Mandelbrot fractal).
 * \param centerX X coordinate of the center of the fractal subset.
 * \param centerY Y coordinate of the center of the fractal subset.
 * \param spanX
 * \param spanY
 * \param escapeRadius Escape radius for computing fractal.
 * \param maxIter Maximum number of iterations for computing fractal.
 */
void InitFractal(Fractal *fractal, FractalFormula fractalFormula, Complex_l p,
		Complex_l c, long double centerX, long double centerY,
		long double spanX, long double spanY, double escapeRadius,
		uint_fast32_t maxIter);

#ifdef _ENABLE_MP_FLOATS
/**
 * \fn void InitFractal2(Fractal *fractal, FractalFormula fractalFormula, const mpc_t p, const mpc_t c, const mpfr_t centerX, const mpfr_t centerY, const mpfr_t spanX, const mpfr_t spanY, double escapeRadius, uint_fast32_t maxIter)
 * \brief Initialize fractal structure.
 *
 * \param fractal Pointer to fractal structure to initialize.
 * \param fractalFormula Fractal type.
 * \param p (main power in iteration) parameter for fractal.
 * \param c Parameter for Julia fractal (will be ignored for Mandelbrot fractal).
 * \param centerX X coordinate of the center of the fractal subset.
 * \param centerY Y coordinate of the center of the fractal subset.
 * \param spanX
 * \param spanY
 * \param escapeRadius Escape radius for computing fractal.
 * \param maxIter Maximum number of iterations for computing fractal.
 */
void InitFractal2(Fractal *fractal, FractalFormula fractalFormula, const mpc_t p, 
			const mpc_t c, const mpfr_t centerX, const mpfr_t centerY,
			const mpfr_t spanX, const mpfr_t spanY,
			double escapeRadius, uint_fast32_t maxIter);
#endif

/**
 * \fn Fractal CopyFractal(const Fractal *fractal)
 * \brief Copy fractal.
 *
 * \param fractal Pointer to fractal to copy.
 * \return Copy of fractal.
 */
Fractal CopyFractal(const Fractal *fractal);

/**
 * \fn int isSupportedFractalFile(const char *fileName)
 * \brief Check whether a file is a supported fractal file.
 *
 * \param fileName File name.
 * \return 1 if file is a supported fractal file, 0 otherwise.
 */
int isSupportedFractalFile(const char *fileName);

/**
 * \fn int ReadFractalFileBody(Fractal *fractal, const char *fileName, FILE *file, const char *format)
 * \brief Create fractal from fractal file body.
 *
 * The body of a fractal file is everything that comes after
 * the format version.\n
 * fileName is used only for error messages.\n
 * This function should only be used internally by the library.
 *
 * \param fractal Pointer to the fractal structure to create.
 * \param fileName Fractal file name.
 * \param file Pointer to opened file, positioned at the beginning of the body.
 * \param format Fractal file format.
 * \return 0 in case of success, 1 in case of failure.
 */
int ReadFractalFileBody(Fractal *fractal, const char *fileName, FILE *file, const char *format);

/**
 * \fn int ReadFractalFile(Fractal *fractal, const char *fileName)
 * \brief Read and parse a fractal file, and create the according fractal.
 *
 * \param fractal Pointer to the fractal structure to create.
 * \param fileName Fractal file name.
 * \return 0 in case of success, 1 in case of failure.
 */
int ReadFractalFile(Fractal *fractal, const char *fileName);

/**
 * \fn int WriteFractalFileBody(const Fractal *fractal, const char *fileName, FILE *file, const char *format)
 * \brief Write fractal file body.
 *
 * The body of a fractal file is everything that comes after
 * the format version.\n
 * fileName is used only for error messages.\n
 * This function should only be used internally by the library.
 *
 * \param fractal Fractal to write into file.
 * \param fileName Fractal file name.
 * \param file Pointer to opened file, positioned at the beginning of the body.
 * \param format Fractal file format.
 * \return 0 in case of success, 1 in case of failure.
 */
int WriteFractalFileBody(const Fractal *fractal, const char *fileName, FILE *file, const char *format);

/**
 * \fn int WriteFractalFile(const Fractal *fractal, const char *fileName)
 * \brief Read and parse a fractal file, and create the according fractal.
 *
 * \param fractal Fractal to write into file.
 * \param fileName Fractal file name.
 * \return 0 in case of success, 1 in case of failure.
 */
int WriteFractalFile(const Fractal *fractal, const char *fileName);

/**
 * \fn void DrawFractal(Image *image, const Fractal *fractal, const RenderingParameters *render, uint_fast32_t quadInterpolationSize, double interpolationThreshold, FloatPrecision floatPrecision, FractalCache *cache, Threads* threads)
 * \brief Draw fractal in a fast, approximate way.
 *
 * Image width and height must be >= 2 (does nothing otherwise).\n
 * Details on the algorithm :
 * The image is cut in quads (rectangles, actually) of size
 * quadInterpolationSize (meaning width AND height <= size).\n
 * Then for each quad, its corner colors are computed, and depending
 * on its dissimilarity (average difference of the corner colors to the
 * average color of the corners), the quad is either computed or linearly
 * interpolated.\n
 * Default values of quadInterpolationSize and interpolationThreshold
 * are good for no visible loss of quality.\n
 * Pointer to cache structure can be NULL if no cache is to be used.\n
 * If cache is not NULL, it must point to a created cache structure,
 * and it is used to generate a preview of the image, and speed-up
 * the task by using values computed by a previous fractal drawing
 * or anti-aliasing.
 *
 * \param image Image in which to draw fractal subset.
 * \param fractal Fractal subset to compute.
 * \param render Rendering parameters.
 * \param quadInterpolationSize Maximum quad size for interpolation.
 * \param interpolationThreshold Dissimilarity threshold for interpolation.
 * \param floatPrecision Float precision.
 * \param cache Cache structure to put computed values in.
 * \param threads Threads to be used for task.
 */
void DrawFractal(Image *image, const Fractal *fractal, const RenderingParameters *render,
			uint_fast32_t quadInterpolationSize, double interpolationThreshold,
			FloatPrecision floatPrecision, FractalCache *cache, Threads* threads);

/**
 * \fn Task *CreateDrawFractalTask(Image *image, const Fractal *fractal, const RenderingParameters *render, uint_fast32_t quadInterpolationSize, double interpolationThreshold, FloatPrecision floatPrecision, FractalCache *cache, uint_fast32_t nbThreads)
 * \brief Create fractal drawing task.
 *
 * Create task and return immediately.\n
 * Task structure can be used to query progress, send
 * cancellation request, etc.\n
 * Image width and height must be >= 2 (does nothing otherwise).\n
 * When launching task, Threads structure should provide
 * enough threads (at least number specified here).
 * Pointer to cache structure can be NULL if no cache is to be used.
 *
 * \param image Image in which to draw fractal subset.
 * \param fractal Fractal subset to compute.
 * \param render Rendering parameters.
 * \param quadInterpolationSize Maximum quad size for interpolation.
 * \param interpolationThreshold Dissimilarity threshold for interpolation.
 * \param floatPrecision Float precision.
 * \param cache Cache structure to put computed values in.
 * \param nbThreads Number of threads that action will need to be launched.
 * \return Corresponding newly-allocated task.
 */
Task *CreateDrawFractalTask(Image *image, const Fractal *fractal, const RenderingParameters *render,
				uint_fast32_t quadInterpolationSize, double interpolationThreshold,
				FloatPrecision floatPrecision,  FractalCache *cache,
				uint_fast32_t nbThreads);

/**
 * \fn void AntiAliaseFractal(Image *image, const Fractal *fractal, const RenderingParameters *render, uint_fast32_t antiAliasingSize, double threshold, FloatPrecision floatPrecision, FractalCache *cache, Threads *threads)
 * \brief AntiAliase fractal image.
 *
 * Image width and height must be >= 2 (does nothing otherwise).\n
 * Anti-aliasing size must be >= 2 to have an effect (does nothing otherwise).
 *
 * Details on the algorithm :
 * Pixels that differ too much from neighbour pixels
 * (difference greater than threshold) are recomputed.\n
 * Several pixels (antiAliasingSize^2 to be precise) are computed
 * for each of these preselected pixels, and averaged (with gaussian
 * filter) to produce the new pixel value.\n
 * Default threshold value is good to obtain a result similar to
 * oversampling (computing a bigger image and downscaling it) with
 * the same size factor.\n
 * Pointer to cache structure can be NULL if no cache is to be used.\n
 * Note that for anti-aliasing, cache is not used to generate of preview
 * of the image and speed-up task: it is only filled with the values
 * computed when anti-aliasing.
 *
 * \param image Fractal image (already drawn) to anti-aliase.
 * \param fractal Fractal subset to compute.
 * \param render Rendering parameters.
 * \param antiAliasingSize Anti-aliasing size.
 * \param threshold Dissimilarity threshold to determine pixels to recompute.
 * \param floatPrecision Float precision.
 * \param cache Cache structure to put computed values in.
 * \param threads Threads to be used for task.
 */
void AntiAliaseFractal(Image *image, const Fractal *fractal, const RenderingParameters *render,
			uint_fast32_t antiAliasingSize, double threshold,
			FloatPrecision floatPrecision, FractalCache *cache, Threads *threads);

/**
 * \fn Task *CreateAntiAliaseFractalTask(Image *image, const Fractal *fractal, const RenderingParameters *render, uint_fast32_t antiAliasingSize, double threshold, FloatPrecision floatPrecision, FractalCache *cache, uint_fast32_t nbThreads)
 * \brief Create task anti-aliasing fractal image
 *
 * Create task and return immediately.\n
 * Image width and height must be >= 2 (does nothing otherwise).\n
 * Anti-aliasing size must be >= 2 to have an effect (does nothing otherwise).\n
 * When launching task, Threads structure should provide
 * enough threads (at least number specified here).
 *
 * \param image Fractal image (already drawn) to anti-aliase.
 * \param fractal Fractal subset to compute.
 * \param render Rendering parameters.
 * \param antiAliasingSize Anti-aliasing size.
 * \param threshold Dissimilarity threshold to determine pixels to recompute.
 * \param floatPrecision Float precision.
 * \param cache Cache structure to put computed values in.
 * \param nbThreads Number of threads that action will need to be launched.
 * \return Corresponding newly-allocated task.
 */
Task *CreateAntiAliaseFractalTask(Image *image, const Fractal *fractal,
					const RenderingParameters *render, uint_fast32_t antiAliasingSize,
					double threshold, FloatPrecision floatPrecision,
					FractalCache *cache, uint_fast32_t nbThreads);

/**
 * \fn void FreeFractal(Fractal fractal)
 * \brief Free a fractal structure.
 *
 * \param fractal Fractal structure to be freed.
 */
void FreeFractal(Fractal fractal);

#ifdef __cplusplus
}
#endif

#endif
