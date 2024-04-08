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
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_H__
#define __FRACTAL_H__

#include "floating_point.h"
#include "float_table.h"
#include "gradient.h"
#include "image.h"
#include "rectangle.h"
#include "rendering_parameters.h"
#include "thread.h"
#include <complex.h>
#include <stdint.h>

/**
 * \def DEFAULT_QUAD_INTERPOLATION_SIZE
 * \brief Default maximum size of quadrilaterals for interpolation.
 *
 * \see ComputeFractalFast for more details.
 */
#define DEFAULT_QUAD_INTERPOLATION_SIZE 10

/**
 * \def DEFAULT_QUAD_DISSIMILARITY_THRESHOLD
 * \brief Default dissimilarity threshold for quad interpolation.
 *
 * \see ComputeFractalFast for more details.
 */
#define DEFAULT_QUAD_DISSIMILARITY_THRESHOLD (FLOAT)(1.187988281E-1)

/**
 * \enum e_FractalType
 * \brief Type of fractal.
 */
typedef enum e_FractalType {
	FRAC_MANDELBROT = 0,
 /*!< Mandelbrot fractal.*/
	FRAC_JULIA
 /*!< Julia fractal.*/
} FractalType;

struct s_Fractal;
typedef struct s_Fractal Fractal;

/**
 * \typedef FractalComputeFunction
 * \brief Computation function for fractal (different for Julia & Mandelbrot).
 */
typedef FLOAT (*FractalComputeFunction)(Fractal *fractal, FLOAT x, FLOAT y);

/**
 * \struct s_Fractal
 * \brief A subset of some fractal set.
 *
 * Describes a subset of some fractal set, plus some parameters
 * for computation.
 */
struct s_Fractal {
	FractalComputeFunction computeFunction;
 /*!< Computation function of fractal.*/
	FLOAT complex c;
 /*!< Parameter for Julia fractal.*/
	FLOAT x1;
 /*!< X coordinate of the upper left point of the fractal subset.*/
	FLOAT y1;
 /*!< Y coordinate of the upper left point of the fractal subset.*/
	FLOAT x2;
 /*!< X coordinate of the down left point of the fractal subset. */
	FLOAT y2;
 /*!< Y coordinate of the down left point of the fractal subset. */
	FLOAT escapeRadius;
 /*!< Escape radius for computing fractal.*/
	uint32_t maxIter;
 /*!< Maximum number of iterations for computing fractal.*/
};

/**
 * \fn void InitFractal(Fractal *fractal, FractalType fractalType, FLOAT complex c, FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT escapeRadius, uint32_t maxIter)
 * \brief Initialize fractal structure.
 *
 * \param fractal Pointer to fractal structure to initialize.
 * \param fractalType Fractal type.
 * \param c Parameter for Julia fractal (will be ignored for Mandelbrot fractal).
 * \param x1 X coordinate of the upper left point of the fractal subset.
 * \param y1 Y coordinate of the upper left point of the fractal subset.
 * \param x2 X coordinate of the down left point of the fractal subset.
 * \param y2 Y coordinate of the down left point of the fractal subset.
 * \param escapeRadius Escape radius for computing fractal.
 * \param maxIter Maximum number of iterations for computing fractal.
 */
void InitFractal(Fractal *fractal, FractalType fractalType, FLOAT complex c, FLOAT x1,
		FLOAT y1, FLOAT x2, FLOAT y2, FLOAT escapeRadius, uint32_t maxIter);

/**
 * \fn void InitFractal2(Fractal *fractal, FractalType fractalType, FLOAT complex c, FLOAT centerX, FLOAT centerY, FLOAT spanX, FLOAT spanY, FLOAT escapeRadius, uint32_t maxIter)
 * \brief Initialize fractal structure.
 *
 * \param fractal Pointer to fractal structure to initialize.
 * \param fractalType Fractal type.
 * \param c Parameter for Julia fractal (will be ignored for Mandelbrot fractal).
 * \param centerX X coordinate of the center of the fractal subset.
 * \param centerY Y coordinate of the center of the fractal subset.
 * \param spanX
 * \param spanY
 * \param escapeRadius Escape radius for computing fractal.
 * \param maxIter Maximum number of iterations for computing fractal.
 */
void InitFractal2(Fractal *fractal, FractalType fractalType, FLOAT complex c, FLOAT centerX,
		FLOAT centerY, FLOAT spanX, FLOAT spanY, FLOAT escapeRadius, uint32_t maxIter);

/**
 * \fn void ReadFractalFile(Fractal *fractal, char *fileName)
 * \brief Read and parse a fractal file, and create the according fractal.
 *
 * \param fractal Pointer to the fractal structure to create.
 * \param fileName Fractal file name.
 */
void ReadFractalFile(Fractal *fractal, char *fileName);

/**
 * \fn FLOAT ComputeFractalValue(Fractal *fractal, FLOAT complex z0, FLOAT complex c);
 * \brief Compute some particular point of fractal.
 *
 * \param fractal Fractal to compute.
 * \param z0 Initial z value.
 * \param c c parameter.
 * @return Value of fractal at specified point.
 */
FLOAT ComputeFractalValue(Fractal *fractal, FLOAT complex z0, FLOAT complex c);

/**
 * \fn FLOAT QuadLinearInterpolation(FLOAT v1, FLOAT v2, FLOAT v3, FLOAT v4, FLOAT x, FLOAT y)
 * \brief Interpolate linearly some value of a quadrilateral.
 *
 * Interpolate value at point (x,y) according to its corner values.
 * Coordinates x and y are relative (i.e. must be between 0 and 1) :
 * (0,0 is the top left corner, (1,0) the top right, etc...
 *
 * \param v Quadrilateral corner values
 * \param x X (relative) coordinate of quad point to interpolate.
 * \param y Y (relative) coordinate of quad point to interpolate.
 * \return Linearly interpolated value at point (x,y) of quadrilateral.
 */
FLOAT QuadLinearInterpolation(FLOAT v[4], FLOAT x, FLOAT y);

/**
 * \fn void ComputeFractalFast(FloatTable *fractal_table, Fractal *fractal,  uint32_t quadInterpolationSize, FLOAT interpolationThreshold)
 * \brief Compute fractal in a fast, approximate way.
 *
 * Details on the algorithm :
 * The float table is cut in quads (rectangles, actually) of size
 * quadInterpolationSize (meaning width AND height <= size).
 * Then for each quad, its corner values are computed, and depending
 * on its dissimilarity (average difference of the corner values to the
 * average value of the corners), the quad is either computed or linearly
 * interpolated.
 * Default values of quadInterpolationSize and interpolationThreshold
 * are good for no visible loss of quality.
 * This function is parallelized : number of threads is given
 * by nbThreads variable. \see threads.h
 *
 * \param fractal_table Float table in which to compute fractal subset.
 * \param fractal Fractal subset to compute.
 * \param quadInterpolationSize Maximum quad size for interpolation.
 * \param interpolationThreshold Dissimilarity threshold for interpolation.
 */
void ComputeFractalFast(FloatTable *fractal_table, Fractal *fractal,
			uint32_t quadInterpolationSize, FLOAT interpolationThreshold);

/**
 * \fn void ComputeFractal(FloatTable *fractal_table, Fractal *fractal)
 * \brief Compute fractal in a (slow) non-approximate way.
 *
 * Note that this is equivalent to ComputeFractalFast(fractal_table,
 * fractal, 1, 0).
 *
 * \param fractal_table Float table in which to compute fractal subset.
 * \param fractal Fractal subset to compute.
 */
void ComputeFractal(FloatTable *fractal_table, Fractal *fractal);

/**
 * \fn void RenderFractal(Image *image, FloatTable *fractal_table, Gradient *gradient, Color spaceColor, FLOAT multiplier)
 * \brief Render float table of a fractal into an image.
 *
 * Float table is typically the output of ComputeFractalFast/
 * ComputeFractal).
 *
 * \param image Image into which to render fractal.
 * \param fractal_table Fractal float table to render.
 * \param param Rendering parameters.
 */
void RenderFractal(Image *image, FloatTable *fractal_table, RenderingParameters *param);

#endif
