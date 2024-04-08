/*
 *  fractal_compute_engine.c -- part of FractalNow
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

#include "float_precision.h"
#include "fractal_compute_engine.h"
#include "fractal_coloring.h"
#include "fractal_formula.h"
#include "fractal.h"
#include "fractal_rendering_parameters.h"
#include "macro_build_fractals.h"
#include "misc.h"
#include <float.h>

#define BUILD_FRACTAL_ENGINE(formula,ptype,coloring,iterationcount,addend,interpolation,fprec) \
struct FractalEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec {\
	FLOATTYPE(FP_##fprec) centerX;\
	FLOATTYPE(FP_##fprec) centerY;\
	FLOATTYPE(FP_##fprec) spanX;\
	FLOATTYPE(FP_##fprec) spanY;\
	FLOATTYPE(FP_##fprec) x1;\
	FLOATTYPE(FP_##fprec) y1;\
	uint_fast32_t n;\
	COMPLEX_FLOATTYPE(FP_##fprec) pixel;\
	FLOATTYPE(FP_##fprec) rePixel;\
	FLOATTYPE(FP_##fprec) imPixel;\
	COMPLEX_FLOATTYPE(FP_##fprec) z;\
	COMPLEX_FLOATTYPE(FP_##fprec) c;\
	FLOATTYPE(FP_##fprec) res;\
	FLOATTYPE(FP_##fprec) normZ;\
	FLOATTYPE(FP_##fprec) escapeRadius;\
	FLOATTYPE(FP_##fprec) escapeRadius2;\
	COMPLEX_FLOATTYPE(FP_##fprec) fractalP;\
	uint_fast32_t fractalP_INT;\
	COMPLEX_FLOATTYPE(FP_##fprec) fractalC;\
	ENGINE_DECL_VAR_FRAC_##formula(FP_##fprec)\
	ENGINE_DECL_VAR_CM_##coloring(IC_##iterationcount,AF_##addend,IM_##interpolation,FP_##fprec)\
};\
\
CacheEntry FractalLoop##formula##ptype##coloring##iterationcount##addend##interpolation##fprec(\
	void *engData, const Fractal *fractal, const RenderingParameters *render,\
	uint_fast32_t x, uint_fast32_t y,\
	uint_fast32_t width, uint_fast32_t height)\
{\
	struct FractalEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec *data =\
	(struct FractalEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec *)engData;\
	UNUSED(render);\
\
	CacheEntry dres;\
	fromUiF(FP_##fprec, data->rePixel, x);\
	add_dF(FP_##fprec, data->rePixel, data->rePixel, 0.5);\
	mulF(FP_##fprec, data->rePixel, data->rePixel, data->spanX);\
	div_uiF(FP_##fprec, data->rePixel, data->rePixel, width);\
	addF(FP_##fprec, data->rePixel, data->rePixel, data->x1);\
\
	fromUiF(FP_##fprec, data->imPixel, y);\
	add_dF(FP_##fprec, data->imPixel, data->imPixel, 0.5);\
	mulF(FP_##fprec, data->imPixel, data->imPixel, data->spanY);\
	div_uiF(FP_##fprec, data->imPixel, data->imPixel, height);\
	addF(FP_##fprec, data->imPixel, data->imPixel, data->y1);\
	cfromReImF(FP_##fprec,data->pixel,data->rePixel,data->imPixel);\
	cfromUiF(FP_##fprec,data->z, 0);\
	cfromUiF(FP_##fprec,data->c, 0);\
	fromUiF(FP_##fprec,data->res,0);\
	fromUiF(FP_##fprec,data->normZ,0);\
	LOOP_INIT_FRAC_##formula(FP_##fprec)\
	LOOP_INIT_CM_##coloring(IC_##iterationcount,AF_##addend,IM_##interpolation,FP_##fprec)\
	for (data->n=0; data->n<fractal->maxIter && \
			cmpF(FP_##fprec,data->normZ,data->escapeRadius2) < 0; ++data->n) {\
		LOOP_ITERATION_CM_##coloring(IC_##iterationcount,AF_##addend,IM_##interpolation,FP_##fprec)\
		LOOP_ITERATION_FRAC_##formula(ptype,FP_##fprec)\
		cnormF(FP_##fprec,data->normZ,data->z);\
	}\
	/* Color even the last iteration, when |z| becomes > escape radius */\
	LOOP_ITERATION_CM_##coloring(IC_##iterationcount,AF_##addend,IM_##interpolation,FP_##fprec)\
	dres.floatPrecision = FP_##fprec;\
	initF(FP_##fprec, dres.x.val_FP_##fprec);\
	initF(FP_##fprec, dres.y.val_FP_##fprec);\
	crealF(FP_##fprec, dres.x.val_FP_##fprec, data->pixel);\
	cimagF(FP_##fprec, dres.y.val_FP_##fprec, data->pixel);\
	if (cmpF(FP_##fprec,data->normZ,data->escapeRadius2) < 0) {\
		dres.value = -1;\
	} else {\
		LOOP_END_CM_##coloring(IC_##iterationcount,AF_##addend,IM_##interpolation,FP_##fprec)\
		dres.value = toDoubleF(FP_##fprec,data->res);\
	}\
	return dres;\
}\
\
void FreeEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec(void *engData)\
{\
	struct FractalEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec *data =\
	(struct FractalEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec *)engData;\
	UNUSED(data);\
\
	clearF(FP_##fprec, data->centerX);\
	clearF(FP_##fprec, data->centerY);\
	clearF(FP_##fprec, data->spanX);\
	clearF(FP_##fprec, data->spanY);\
	clearF(FP_##fprec, data->x1);\
	clearF(FP_##fprec, data->y1);\
	cclearF(FP_##fprec, data->pixel);\
	clearF(FP_##fprec, data->rePixel);\
	clearF(FP_##fprec, data->imPixel);\
	cclearF(FP_##fprec, data->z);\
	cclearF(FP_##fprec, data->c);\
	clearF(FP_##fprec, data->res);\
	clearF(FP_##fprec, data->normZ);\
	clearF(FP_##fprec, data->escapeRadius);\
	clearF(FP_##fprec, data->escapeRadius2);\
	cclearF(FP_##fprec, data->fractalP);\
	cclearF(FP_##fprec, data->fractalC);\
	ENGINE_CLEAR_VAR_FRAC_##formula(FP_##fprec)\
	ENGINE_CLEAR_VAR_CM_##coloring(IC_##iterationcount,AF_##addend,IM_##interpolation,FP_##fprec)\
}\
\
void InitEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec(\
		const Fractal *fractal, const RenderingParameters *render, FractalEngine *engine)\
{\
	UNUSED(render);\
	engine->fractalLoop = FractalLoop##formula##ptype##coloring##iterationcount##addend##interpolation##fprec;\
	engine->freeEngineData = FreeEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec;\
	engine->data = (struct FractalEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec *)\
		safeMalloc("fractal engine",\
		sizeof(struct FractalEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec));\
	struct FractalEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec *data =\
	(struct FractalEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec *)engine->data;\
	UNUSED(data);\
\
	initF(FP_##fprec, data->centerX);\
	initF(FP_##fprec, data->centerY);\
	initF(FP_##fprec, data->spanX);\
	initF(FP_##fprec, data->spanY);\
	initF(FP_##fprec, data->x1);\
	initF(FP_##fprec, data->y1);\
	cinitF(FP_##fprec, data->pixel);\
	initF(FP_##fprec, data->rePixel);\
	initF(FP_##fprec, data->imPixel);\
	cinitF(FP_##fprec, data->z);\
	cinitF(FP_##fprec, data->c);\
	initF(FP_##fprec, data->res);\
	initF(FP_##fprec, data->normZ);\
	initF(FP_##fprec, data->escapeRadius);\
	initF(FP_##fprec, data->escapeRadius2);\
	cinitF(FP_##fprec, data->fractalP);\
	cinitF(FP_##fprec, data->fractalC);\
	fromMPFRF(FP_##fprec, data->centerX, fractal->centerX);\
	fromMPFRF(FP_##fprec, data->centerY, fractal->centerY);\
	fromMPFRF(FP_##fprec, data->spanX, fractal->spanX);\
	fromMPFRF(FP_##fprec, data->spanY, fractal->spanY);\
	fromMPFRF(FP_##fprec, data->x1, fractal->x1);\
	fromMPFRF(FP_##fprec, data->y1, fractal->y1);\
	fromDoubleF(FP_##fprec, data->escapeRadius, fractal->escapeRadius);\
	mulF(FP_##fprec, data->escapeRadius2, data->escapeRadius, data->escapeRadius);\
	cfromMPCF(FP_##fprec, data->fractalP, fractal->p);\
	cfromMPCF(FP_##fprec, data->fractalC, fractal->c);\
	data->fractalP_INT = (uint_fast32_t)creal(ctoCDoubleF(FP_##fprec, data->fractalP));\
	ENGINE_INIT_VAR_FRAC_##formula(FP_##fprec)\
	ENGINE_INIT_VAR_CM_##coloring(IC_##iterationcount,AF_##addend,IM_##interpolation,FP_##fprec)\
}

#undef MACRO_BUILD_FRACTAL

/* All this crap to avoid creating duplicates of fractal engines:
   Indeed, with CM_ITERATIONCOUNT coloring method, addend function
   and interpolation methods are not used : we generate only one
   fractal engine for all interpolation methods and addend functions
   (for AF_TRIANGLEINEQUALITY and IM_NONE).
   The same thing applies with CM_AVERAGECOLORING color methods,
   where iteration count is not used : we generate only one fractal
   engine for all iteration counts.
   It is necessary to reduce executable size... When we are able
   to compile loops at runtime we won't need all these macros.*/
#define BUILD_FRACTAL_ENGINE_CM_ITERATIONCOUNT_AF_TRIANGLEINEQUALITY_IM_NONE(formula,ptype,iterationcount,fprec) \
	BUILD_FRACTAL_ENGINE(formula,ptype,ITERATIONCOUNT,iterationcount,TRIANGLEINEQUALITY,NONE,fprec)
#define BUILD_FRACTAL_ENGINE_CM_ITERATIONCOUNT_AF_TRIANGLEINEQUALITY_IM_LINEAR(formula,ptype,iterationcount,fprec)
#define BUILD_FRACTAL_ENGINE_CM_ITERATIONCOUNT_AF_TRIANGLEINEQUALITY_IM_SPLINE(formula,ptype,iterationcount,fprec)

#define BUILD_FRACTAL_ENGINE_CM_ITERATIONCOUNT_AF_TRIANGLEINEQUALITY(formula,ptype,iterationcount,interpolation,fprec) \
	BUILD_FRACTAL_ENGINE_CM_ITERATIONCOUNT_AF_TRIANGLEINEQUALITY_IM_##interpolation(\
		formula,ptype,iterationcount,fprec)
#define BUILD_FRACTAL_ENGINE_CM_ITERATIONCOUNT_AF_CURVATURE(formula,ptype,iterationcount,interpolation,fprec)
#define BUILD_FRACTAL_ENGINE_CM_ITERATIONCOUNT_AF_STRIPE(formula,ptype,iterationcount,interpolation,fprec)

#define BUILD_FRACTAL_ENGINE_CM_ITERATIONCOUNT(formula,ptype,coloring,iterationcount,addend,interpolation,fprec) \
	BUILD_FRACTAL_ENGINE_CM_ITERATIONCOUNT_AF_##addend(formula,ptype,iterationcount,interpolation,fprec)

#define BUILD_FRACTAL_ENGINE_CM_AVERAGECOLORING_IC_DISCRETE(formula,ptype,addend,interpolation,fprec) \
	BUILD_FRACTAL_ENGINE(formula,ptype,AVERAGECOLORING,DISCRETE,addend,interpolation,fprec)
#define BUILD_FRACTAL_ENGINE_CM_AVERAGECOLORING_IC_CONTINUOUS(formula,ptype,addend,interpolation,fprec)
#define BUILD_FRACTAL_ENGINE_CM_AVERAGECOLORING_IC_SMOOTH(formula,ptype,addend,interpolation,fprec)

#define BUILD_FRACTAL_ENGINE_CM_AVERAGECOLORING(formula,ptype,coloring,iterationcount,addend,interpolation,fprec) \
	BUILD_FRACTAL_ENGINE_CM_AVERAGECOLORING_IC_##iterationcount(formula,ptype,addend,interpolation,fprec)

#define MACRO_BUILD_FRACTAL(formula,ptype,coloring,iterationcount,addend,interpolation,fprec) \
	BUILD_FRACTAL_ENGINE_CM_##coloring(formula,ptype,coloring,iterationcount,addend,interpolation,fprec)
MACRO_BUILD_FRACTALS

/* Build CreateEngine function. */
#define VAL_PINT 1
#define VAL_PFLOATT 0

/*#define BUILD_InitEngine(formula,ptype,coloring,iterationcount,addend,interpolation,fprec) \
if (fractal->fractalFormula == FRAC_##formula && mpcIsInteger(fractal->p) == VAL_##ptype && \
	render->coloringMethod == CM_##coloring && render->iterationCount == IC_##iterationcount && \
	render->addendFunction == AF_##addend && render->interpolationMethod == IM_##interpolation && \
	floatPrecision == FP_##fprec) {\
\
	InitEngine##formula##ptype##coloring##iterationcount##addend##interpolation##fprec(\
		fractal,render,engine);\
	return;\
}*/

#define BUILD_InitEngine_CM_ITERATIONCOUNT(formula,ptype,iterationcount,addend,interpolation,fprec)\
if (render->coloringMethod == CM_ITERATIONCOUNT && fractal->fractalFormula == FRAC_##formula && \
	mpcIsInteger(fractal->p) == VAL_##ptype && render->iterationCount == IC_##iterationcount && \
	floatPrecision == FP_##fprec) {\
\
	InitEngine##formula##ptype##ITERATIONCOUNT##iterationcount##TRIANGLEINEQUALITY##NONE##fprec(\
		fractal,render,engine);\
	return;\
}

#define BUILD_InitEngine_CM_AVERAGECOLORING(formula,ptype,iterationcount,addend,interpolation,fprec)\
if (render->coloringMethod == CM_AVERAGECOLORING && fractal->fractalFormula == FRAC_##formula && \
	mpcIsInteger(fractal->p) == VAL_##ptype && render->addendFunction == AF_##addend && \
	render->interpolationMethod == IM_##interpolation && floatPrecision == FP_##fprec) {\
\
	InitEngine##formula##ptype##AVERAGECOLORING##DISCRETE##addend##interpolation##fprec(\
		fractal,render,engine);\
	return;\
}

#define BUILD_InitEngine(formula,ptype,coloring,iterationcount,addend,interpolation,fprec) \
	BUILD_InitEngine_CM_##coloring(formula,ptype,iterationcount,addend,interpolation,fprec)

#undef MACRO_BUILD_FRACTAL
#define MACRO_BUILD_FRACTAL(formula,ptype,coloring,iterationcount,addend,interpolation,fprec) \
	BUILD_InitEngine(formula,ptype,coloring,iterationcount,addend,interpolation,fprec)

void CreateFractalEngine(FractalEngine *engine, const Fractal *fractal,
				const RenderingParameters *render, FloatPrecision floatPrecision)
{
	MACRO_BUILD_FRACTALS

	/* This should never happen, because fractal engine is built by macros for all possible
	 * values of each parameter.
	 */
	FractalNow_error("Could not create fractal compute engine for given parameters.\n");
}

void FreeFractalEngine(FractalEngine *engine)
{
	engine->freeEngineData(engine->data);
	free(engine->data);
}

CacheEntry RunFractalEngine(const FractalEngine *engine, const Fractal *fractal,
			const RenderingParameters *render, uint_fast32_t x, uint_fast32_t y,
			uint_fast32_t width, uint_fast32_t height)
{
	return engine->fractalLoop(engine->data, fractal, render, x, y, width, height);
}

