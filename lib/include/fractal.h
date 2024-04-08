/*
 *  fractal.h -- part of fractal2D
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
  * \brief Header file related to fractals.
  *
  * How is a fractal point computed and colored (step by step) :
  * - The fractal loop function is executed, and returns a floating point
  *   value. See fractal loop header for more details on fractal loops.
  *   \see fractal_loop.h
  * - The transfer functions is applied, as well as the multiplier, and the
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

#ifdef __cplusplus
extern "C" {
#endif

#include "floating_point.h"
#include "gradient.h"
#include "image.h"
#include "fractal_formula.h"
#include "fractal_rendering_parameters.h"
#include "thread.h"
#include <complex.h>
#include <stdint.h>

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
#define DEFAULT_COLOR_DISSIMILARITY_THRESHOLD (FLOAT)(3.5E-3)

/**
 * \def DEFAULT_ADAPTIVE_AAM_THRESHOLD
 * \brief Default threshold for adaptive anti-aliasing.
 *
 * \see AntiAliaseFractal for more details.
 */
#define DEFAULT_ADAPTIVE_AAM_THRESHOLD (FLOAT)(5.05E-2)

struct s_Fractal;
typedef struct s_Fractal Fractal;

/**
 * \struct s_Fractal
 * \brief A subset of some fractal set.
 *
 * Describes a subset of some fractal set, plus some parameters
 * used for computation.
 */
struct s_Fractal {
	FractalFormula fractalFormula;
 /*!< Fractal type.*/
	FLOAT p;
 /*!< Parameter for some fractals (main power in iteration z = z^p + ...).*/
	FLOAT complex c;
 /*!< Parameter for Julia and Rudy fractal.*/
	FLOAT x1;
 /*!< X coordinate of the upper left point of the fractal subset.*/
	FLOAT y1;
 /*!< Y coordinate of the upper left point of the fractal subset.*/
	FLOAT x2;
 /*!< X coordinate of the down left point of the fractal subset.*/
	FLOAT y2;
 /*!< Y coordinate of the down left point of the fractal subset.*/
	FLOAT centerX;
 /*!< X coordinate of the center of the fractal subset.*/
	FLOAT centerY;
 /*!< Y coordinate of the center of the fractal subset.*/
	FLOAT spanX;
 /*!< X span of the fractal subset.*/
	FLOAT spanY;
 /*!< Y span of the fractal subset.*/
	FLOAT escapeRadius;
 /*!< Escape radius for computing fractal.*/
	uint_fast32_t maxIter;
 /*!< Maximum number of iterations for computing fractal.*/

 /* Some parameters for internal use.*/
	FLOAT logP;
 /*!< Logarithm of p.*/
	int p_is_integer;
 /*!< 1 if p is integer, 0 otherwise.*/
	uint_fast8_t p_int;
 /*!< p as integer (if p is an integer).*/
	FLOAT escapeRadius2;
 /*!< Escape radius raised to the power of 2.*/
	FLOAT escapeRadiusP;
 /*!< Escape radius raised to the power of p.*/
	FLOAT logEscapeRadius;
 /*!< Logarithm of escape radius.*/
};

/**
 * \fn void InitFractal(Fractal *fractal, FractalFormula fractalFormula, FLOAT p, FLOAT complex c, FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT escapeRadius, uint_fast32_t maxIter)
 * \brief Initialize fractal structure.
 *
 * \param fractal Pointer to fractal structure to initialize.
 * \param fractalFormula Fractal type.
 * \param p (main power in iteration) parameter for fractal.
 * \param c Parameter for Julia fractal (will be ignored for Mandelbrot fractal).
 * \param x1 X coordinate of the upper left point of the fractal subset.
 * \param y1 Y coordinate of the upper left point of the fractal subset.
 * \param x2 X coordinate of the down left point of the fractal subset.
 * \param y2 Y coordinate of the down left point of the fractal subset.
 * \param escapeRadius Escape radius for computing fractal.
 * \param maxIter Maximum number of iterations for computing fractal.
 */
void InitFractal(Fractal *fractal, FractalFormula fractalFormula, FLOAT p, FLOAT complex c,
		FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT escapeRadius,
		uint_fast32_t maxIter);

/**
 * \fn void InitFractal2(Fractal *fractal, FractalFormula fractalFormula, FLOAT p, FLOAT complex c, FLOAT centerX, FLOAT centerY, FLOAT spanX, FLOAT spanY, FLOAT escapeRadius, uint_fast32_t maxIter)
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
void InitFractal2(Fractal *fractal, FractalFormula fractalFormula, FLOAT p, FLOAT complex c,
		FLOAT centerX, FLOAT centerY, FLOAT spanX, FLOAT spanY,
		FLOAT escapeRadius, uint_fast32_t maxIter);

/**
 * \fn Fractal CopyFractal(const Fractal *fractal)
 * \brief Copy fractal.
 *
 * \param fractal Pointer to fractal to copy.
 * \return Copy of fractal.
 */
Fractal CopyFractal(const Fractal *fractal);

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
 * \fn Color ComputeFractalColor(const Fractal *fractal, const RenderingParameters *render, FLOAT complex z);
 * \brief Compute some particular point of fractal.
 *
 * Function will compute the RGB color at given point according
 * to fractal and rendering parameters.
 *
 * \param fractal Fractal to compute.
 * \param render Fractal rendering parameters.
 * \param z Point in the complex plan to compute.
 * @return Color of fractal at specified point.
 */
Color ComputeFractalColor(const Fractal *fractal, const RenderingParameters *render, FLOAT complex z);

/**
 * \fn void DrawFractalFast(Image *image, const Fractal *fractal, const RenderingParameters *render, uint_fast32_t quadInterpolationSize, FLOAT interpolationThreshold)
 * \brief Draw fractal in a fast, approximate way.
 *
 * Image width and height must be >= 2 (does nothing otherwise).
 * Details on the algorithm :
 * The image is cut in quads (rectangles, actually) of size
 * quadInterpolationSize (meaning width AND height <= size).
 * Then for each quad, its corner colors are computed, and depending
 * on its dissimilarity (average difference of the corner colors to the
 * average color of the corners), the quad is either computed or linearly
 * interpolated.
 * Default values of quadInterpolationSize and interpolationThreshold
 * are good for no visible loss of quality.
 * This function is parallelized : number of threads is given
 * by nbThreads variable. \see threads.h
 *
 * \param image Image in which to draw fractal subset.
 * \param fractal Fractal subset to compute.
 * \param render Rendering parameters.
 * \param quadInterpolationSize Maximum quad size for interpolation.
 * \param interpolationThreshold Dissimilarity threshold for interpolation.
 */
void DrawFractalFast(Image *image, const Fractal *fractal, const RenderingParameters *render,
			uint_fast32_t quadInterpolationSize, FLOAT interpolationThreshold);

/**
 * \fn Action *LaunchDrawFractalFast(Image *image, const Fractal *fractal, const RenderingParameters *render, uint_fast32_t quadInterpolationSize, FLOAT interpolationThreshold)
 * \brief Launch fractal drawing but does not wait for termination.
 *
 * Image width and height must be >= 2 (does nothing otherwise).
 * Action returned can be used to wait for termination or cancel drawing.
 *
 * \param image Image in which to draw fractal subset.
 * \param fractal Fractal subset to compute.
 * \param render Rendering parameters.
 * \param quadInterpolationSize Maximum quad size for interpolation.
 * \param interpolationThreshold Dissimilarity threshold for interpolation.
 * \return Corresponding newly-allocated action.
 */
Action *LaunchDrawFractalFast(Image *image, const Fractal *fractal, const RenderingParameters *render,
			uint_fast32_t quadInterpolationSize, FLOAT interpolationThreshold);

/**
 * \fn void DrawFractal(Image *image, const Fractal *fractal, const RenderingParameters *render)
 * \brief Draw fractal in a (slow) non-approximate way.
 *
 * Image width and height must be >= 2 (does nothing otherwise).
 *
 * Note that this is equivalent to DrawFractalFast(fractal_table,
 * fractal, 1, 0).
 *
 * \param image Image in which to draw fractal subset.
 * \param fractal Fractal subset to compute.
 * \param render Rendering parameters.
 */
void DrawFractal(Image *image, const Fractal *fractal, const RenderingParameters *render);

/**
 * \fn void AntiAliaseFractal(Image *image, const Fractal *fractal, const RenderingParameters *render, uint_fast32_t antiAliasingSize, FLOAT threshold)
 * \brief AntiAliase fractal image.
 *
 * Image width and height must be >= 2 (does nothing otherwise).
 * Anti-aliasing size must be >= 2 to have an effect (does nothing otherwise).
 *
 * Details on the algorithm :
 * Pixels that differ too much from neighbour pixels
 * (difference greater than threshold) are recomputed.
 * Several pixels (antiAliasingSize^2 to be precise) are computed
 * for each of these preselected pixels, and averaged (with gaussian
 * filter) to produce the new pixel value.
 * Default threshold value is good to obtain a result similar to
 * oversampling (computing a bigger image and downscaling it) with
 * the same size factor.
 * This function is parallelized : number of threads is given
 * by nbThreads variable. \see threads.h
 *
 * \param image Fractal image (already drawn) to anti-aliase.
 * \param fractal Fractal subset to compute.
 * \param render Rendering parameters.
 * \param antiAliasingSize Anti-aliasing size.
 * \param threshold Dissimilarity threshold to determine pixels to recompute.
 */
void AntiAliaseFractal(Image *image, const Fractal *fractal, const RenderingParameters *render,
			uint_fast32_t antiAliasingSize, FLOAT threshold);

/**
 * \fn Action LaunchAntiAliaseFractal(Image *image, const Fractal *fractal, const RenderingParameters *render, uint_fast32_t antiAliasingSize, FLOAT threshold)
 * \brief Launch fractal image anti-aliasing, but does not wait for termination.
 *
 * Image width and height must be >= 2 (does nothing otherwise).
 * Anti-aliasing size must be >= 2 to have an effect (does nothing otherwise).
 * Action returned can be used to wait for termination or cancel anti-aliasing.
 *
 * \param image Fractal image (already drawn) to anti-aliase.
 * \param fractal Fractal subset to compute.
 * \param render Rendering parameters.
 * \param antiAliasingSize Anti-aliasing size.
 * \param threshold Dissimilarity threshold to determine pixels to recompute.
 * \return Corresponding newly-allocated action.
 */
Action *LaunchAntiAliaseFractal(Image *image, const Fractal *fractal, const RenderingParameters *render,
			uint_fast32_t antiAliasingSize, FLOAT threshold);

#ifdef __cplusplus
}
#endif

#endif
