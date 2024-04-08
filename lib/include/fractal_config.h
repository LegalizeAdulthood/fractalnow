/*
 *  fractal_config.h -- part of FractalNow
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
  * \file fractal_config.h
  * \brief Header file related to fractal configuration structure.
  *
  * A fractal configuration is the conjunction of a fractal and 
  * rendering parameters.
  * A configuration file is more or less the concatenation of a fractal
  * file and a rendering file.
  */

#ifndef __FRACTAL_CONFIG_H__
#define __FRACTAL_CONFIG_H__

#include "fractal.h"
#include "fractal_rendering_parameters.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct FractalConfig
 * \brief Structure containing fractal and fractal rendering parameters.
 */
/**
 * \typedef FractalConfig
 * \brief Convenient typedef for struct FractalConfig.
 */
typedef struct FractalConfig {
	Fractal fractal;
 /*!< Fractal.*/
	RenderingParameters render;
 /*!< Rendering parameters.*/
} FractalConfig;

/**
 * \fn void InitFractalConfig(FractalConfig *config, Fractal fractal, RenderingParameters render)
 * \brief Initialize fractal config.
 *
 * Fractal and rendering parameters will be owned by fractal config,
 * and will be free'd when fractal config is free'd.
 *
 * \param config Pointer to structure to initialize.
 * \param fractal Fractal
 * \param render Rendering parameters.
 */
void InitFractalConfig(FractalConfig *config, Fractal fractal, RenderingParameters render);
	
/**
 * \fn FractalConfig CopyFractalConfig(const FractalConfig *config)
 * \brief Copy fractal configuration.
 *
 * \param config Pointer to fractal configuration to copy.
 * \return Copy of fractal configuration.
 */
FractalConfig CopyFractalConfig(const FractalConfig *config);

/**
 * \fn void ResetFractal(FractalConfig *config, Fractal fractal)
 * \brief Reset fractal.
 *
 * Fractal will be owned by fractal config, and
 * will be free'd when fractal config is free'd.
 *
 * \param config Fractal config to be changed.
 * \param fractal New fractal.
 */
void ResetFractal(FractalConfig *config, Fractal fractal);

/**
 * \fn void ResetRenderingParameters(FractalConfig *config, RenderingParameters render)
 * \brief Reset rendering parameters.
 *
 * RenderingParameters will be owned by fractal config, and
 * will be free'd when fractal config is free'd.
 *
 * \param config Fractal config to be changed.
 * \param render New rendering parameters.
 */
void ResetRenderingParameters(FractalConfig *config, RenderingParameters render);

/**
 * \fn int isSupportedFractalConfigFile(const char *fileName)
 * \brief Check whether a file is a supported fractal configuration file.
 *
 * \param fileName File name.
 * \return 1 if file is a supported fractal config file, 0 otherwise.
 */
int isSupportedFractalConfigFile(const char *fileName);

/**
 * \fn int ReadFractalConfigFileBody(FractalConfig *fractalConfig, const char *fileName, FILE *file, const char *format)
 * \brief Read fractal configuration from fractal configuration file body.
 *
 * The body of a configuration file is everything that comes
 * after the format version.\n
 * fileName is used only for error messages.\n
 * This function should only be used internally by the library.
 *
 * \param fractalConfig Pointer to the configuration file structure to create.
 * \param fileName Fractal configuration file name.
 * \param file Pointer to opened file, positioned at the beginning of the body.
 * \param format Fractal configuration file format.
 * \return 0 in case of success, 1 in case of failure.
 */
int ReadFractalConfigFileBody(FractalConfig *fractalConfig, const char *fileName,
				FILE *file, const char *format);

/**
 * \fn int ReadFractalConfigFile(FractalConfig *fractalConfig, const char *fileName)
 * \brief Read and parse fractal configuration file.
 *
 * \param fractalConfig Pointer to the structure to store fractal configuration.
 * \param fileName Name of file containing fractal configuration.
 * \return 0 in case of success, 1 in case of failure.
 */
int ReadFractalConfigFile(FractalConfig *fractalConfig, const char *fileName);

/**
 * \fn int WriteFractalConfigFileBody(const FractalConfig *fractalConfig, const char *fileName, FILE *file, const char *format)
 * \brief Write fractal configuration file body.
 *
 * The body of a configuration file is everything that comes
 * after the format version.\n
 * fileName is used only for error messages.\n
 * This function should only be used internally by the library.
 *
 * \param fractalConfig Fractal configuration to write.
 * \param fileName Fractal configuration file name.
 * \param file Pointer to opened file, positioned at the beginning of the body.
 * \param format Fractal configuration file format.
 * \return 0 in case of success, 1 in case of failure.
 */
int WriteFractalConfigFileBody(const FractalConfig *fractalConfig, const char *fileName,
				FILE *file, const char *format);

/**
 * \fn int WriteFractalConfigFile(const FractalConfig *fractalConfig, const char *fileName)
 * \brief Write fractal configuration file.
 *
 * \param fractalConfig Fractal configuration file to write.
 * \param fileName Name of file containing fractal configuration.
 * \return 0 in case of success, 1 in case of failure.
 */
int WriteFractalConfigFile(const FractalConfig *fractalConfig, const char *fileName);

/**
 * \fn void FreeFractalConfig(FractalConfig fractalConfig)
 * \brief Free a FractalConfig structure.
 *
 * \param fractalConfig FractalConfig structure to be freed.
 */
void FreeFractalConfig(FractalConfig fractalConfig);

#ifdef __cplusplus
}
#endif

#endif
