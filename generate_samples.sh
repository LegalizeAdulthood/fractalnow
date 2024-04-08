#!/bin/bash
# 
#  generate_samples.sh -- part of FractalNow
# 
#  Copyright (c) 2012 Marc Pegon <pe.marc@free.fr>
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#

QUIET=0;
VERBOSE=0;
DRY_RUN=0;
DEBUG=0;
EXAMPLES_DIR="examples"
FRACTAL_DIR="${EXAMPLES_DIR}/fractal_files"
RENDER_DIR="${EXAMPLES_DIR}/rendering_files"
CONFIG_DIR="${EXAMPLES_DIR}/config_files"
FRACTAL_SUFFIX=".fractal"
RENDER_SUFFIX=".render"
CONFIG_SUFFIX=".config"
IMGS_DIR="imgs"
FRACTAL_FILE_PREFIX="fractal"
RENDER_FILE_PREFIX="render"
EXE="fractalnow"

usage(){
	cat<<EOF

generate_samples.sh [OPTIONS] [FRACTALNOW_OPTIONS]
Run fractalnow on example files to generate image samples.
For configuration file 'foobar.config'
    fractalnow is run on 'foobar.config'.
For each fractal file 'foobar.fractal':
    fractalnow is run multiple times, for each 'foobarXXX.render'
    rendering file.

OPTIONS :
-h                : Print this help
-n                : Dry-run mode (print commands without executing them)
-e <executable>   : Specify full path to fractalnow executable
-f <fractal_dir>  : Specify fractal files directory (default: '${FRACTAL_DIR}')
-r <render_dir>   : Specify rendering files directory (default: '${RENDER_DIR}')
-c <config_dir>   : Specify configuration files directory (default: '${CONFIG_DIR}')
-o <output_dir>   : Specify output directory for image files (default: '${IMGS_DIR}')
FRACTALNOW OPTIONS (see fractalnow help) :
-d 
-q 
-v 
-x <width>
-y <height>
-l <float_type>
-L <MP_float_precision>
-a <aa_method>
-s <aa_size>
-j <nb_threads>
EOF
}

function error {
	echo $1 >> /dev/stderr;

	if [ ! -z $2 ]; then
		exit $2;
	else
		exit 1;
	fi;
}

while getopts ":hc:e:f:r:o:qvdnx:y:a:s:j:l:L:" opt; do
	case "$opt" in
		'h')
			usage;
			exit 1;
		;;
		'c')
			CONFIG_DIR="$OPTARG";
		;;
		'e')
			EXE="$OPTARG";
		;;
		'f')
			FRACTAL_DIR="$OPTARG";
		;;
		'r')
			RENDER_DIR="$OPTARG";
		;;
		'o')
			IMGS_DIR="$OPTARG";
		;;
		'q')
			QUIET=1;
		;;
		'v')
			VERBOSE=1;
		;;
		'd')
			DEBUG=1;
		;;
		'n')
			DRY_RUN=1;
		;;
		'x')
			if eval [ "$OPTARG" -eq "$OPTARG" 2>/dev/null ]; then
				WIDTH=`eval echo "$OPTARG"`;
			else
				error 'Width is not a number.';
			fi;
		;;
		'y')
			if eval [ "$OPTARG" -eq "$OPTARG" 2>/dev/null ]; then
				HEIGHT=`eval echo "$OPTARG"`;
			else
				error 'Height is not a number.';
			fi;
		;;
		'l')
			FLOATTYPE="$OPTARG";
		;;
		'L')
			MPFLOATPRECISION="$OPTARG";
		;;
		'a')
			AAM="$OPTARG";
		;;
		's')
			if eval [ "$OPTARG" -eq "$OPTARG" 2>/dev/null ]; then
				AAM_SIZE=`eval echo "$OPTARG"`;
			else
				error 'AAM_Size is not a number.';
			fi;
		;;
		'j')
			if eval [ "$OPTARG" -eq "$OPTARG" 2>/dev/null ]; then
				NB_THREADS=`eval echo "$OPTARG"`;
			else
				error 'Number of threads is not a number.';
			fi;
		;;
		*)
			echo "Invalid option '$OPTARG'." >&2;
			usage;
			exit 1;
		;;
	esac;
done;

shift $(($OPTIND - 1))

if [ $# -ne 0 ]; then
	echo "Unknown arguments remaining on command line.";
	usage;
	exit 1;
fi;

if [ -z "${WIDTH}" -a -z "${HEIGHT}" ]; then
	echo "At least width or height should be specified.";
	usage;
	exit 1;
fi;

`${EXE} -h &>/dev/null`
if [ $? -ne 0 ]; then 
	echo "Could not find '${EXE}' executable."
	exit 1;
fi;

if [ ${DRY_RUN} -eq 0 ]; then
	function cmd {
		echo "$@";
		eval "$@";
	}
else
	function cmd {
		echo "$@";
	}
fi;

# Make sure that FRACTAL_DIR exists and is a directory.
if [ ! -e "${FRACTAL_DIR}" ]; then
	error "'${FRACTAL_DIR}' doesn't exist.";
elif [ ! -d "${FRACTAL_DIR}" ]; then
	error "'${FRACTAL_DIR}' is not a directory.";
fi;

# Make sure that RENDER_DIR is a directory.
if [ ! -e "${RENDER_DIR}" ]; then
	error "'${RENDER_DIR}' doesn't exist.";
elif [ ! -d "${RENDER_DIR}" ]; then
	error "'${RENDER_DIR}' is not a directory.";
fi;

# Check whether IMGS_DIR exists or not
if [ -e "${IMGS_DIR}" ]; then
# IMGS_DIR already exists, make sure it is a directory.
	if [ ! -d "${IMGS_DIR}" ]; then
		error "'${IMGS_DIR}' is not a directory.";
	fi;
else
# IMGS_DIR doesn't exist, create it.
	cmd mkdir -p "\"${IMGS_DIR}\"";
fi;

#Set fractalnow flags
if [ ${VERBOSE} -eq 1 ]; then
	FLAGS="$FLAGS -v";
fi;
if [ ${QUIET} -eq 1 ]; then
	FLAGS="$FLAGS -q";
fi;
if [ ! -z ${NB_THREADS} ]; then
	FLAGS="$FLAGS -j ${NB_THREADS}";
fi;
if [ ! -z ${WIDTH} ]; then
	FLAGS="$FLAGS -x ${WIDTH}";
	IMG_SUFFIX="${IMG_SUFFIX}_x${WIDTH}";
fi;
if [ ! -z ${HEIGHT} ]; then
	FLAGS="$FLAGS -y ${HEIGHT}";
	IMG_SUFFIX="${IMG_SUFFIX}_y${HEIGHT}";
fi;
if [ ! -z ${FLOATTYPE} ]; then
	FLAGS="$FLAGS -l ${FLOATTYPE}";
	IMG_SUFFIX="${IMG_SUFFIX}_l${FLOATTYPE}";
fi
if [ ! -z ${MPFLOATPRECISION} ]; then
	FLAGS="$FLAGS -L ${MPFLOATPRECISION}";
	IMG_SUFFIX="${IMG_SUFFIX}_L${MPFLOATPRECISION}";
fi
if [ ! -z ${AAM} ]; then
	FLAGS="$FLAGS -a ${AAM}";
	IMG_SUFFIX="${IMG_SUFFIX}_a${AAM}";
fi;
if [ ! -z ${AAM_SIZE} ]; then
	FLAGS="$FLAGS -s ${AAM_SIZE}";
	IMG_SUFFIX="${IMG_SUFFIX}_s${AAM_SIZE}";
fi;
IMG_SUFFIX="${IMG_SUFFIX}.ppm";

fractal_files=`eval ls \"${FRACTAL_DIR}\" --quoting-style=c -1 | grep ^\""${FRACTAL_FILE_PREFIX}.*${FRACTAL_SUFFIX}"\"$ | sort`;

IFS=$'\n';
for i in ${fractal_files} ; do
	fractal_file=`eval echo -e "${i}"`;
	fractal_file_basename=`basename "${fractal_file}" "${FRACTAL_SUFFIX}"`;
	fractal_file_suffix=`eval echo \"${fractal_file_basename}\" | sed "s/.*${FRACTAL_FILE_PREFIX}\(.*\)/\1/"`
	current_render_prefix="${RENDER_FILE_PREFIX}${fractal_file_suffix}"
	render_files=`eval ls \"${RENDER_DIR}\" --quoting-style=c | grep ^\""${current_render_prefix}.*${RENDER_SUFFIX}" | sort`;
	for j in ${render_files} ; do
		render_file=`eval echo -e "$j"`;
		render_file_basename=`basename "${render_file}" "${RENDER_SUFFIX}"`;
		cmd \"${EXE}\" -f \"${FRACTAL_DIR}/${fractal_file}\" -r \"${RENDER_DIR}/${render_file}\" -o \"${IMGS_DIR}/${fractal_file_basename}_${render_file_basename}${IMG_SUFFIX}\" ${FLAGS};
		if [ $? -ne 0 ]; then
			error "Error occured when running executable. Interrupting."
		fi;
		echo;
	done;
done;

config_files=`eval ls \"${CONFIG_DIR}\" --quoting-style=c -1 | grep ^\""${CONFIG_FILE_PREFIX}.*${CONFIG_SUFFIX}"\"$ | sort`;
for i in ${config_files} ; do
	config_file=`eval echo -e "${i}"`;
	config_file_basename=`basename "${config_file}" "${CONFIG_SUFFIX}"`;
	cmd \"${EXE}\" -c \"${CONFIG_DIR}/${config_file}\" -o \"${IMGS_DIR}/${config_file_basename}${IMG_SUFFIX}\" ${FLAGS};
	if [ $? -ne 0 ]; then
		error "Error occured when running executable. Interrupting."
	fi;
	echo;
done;
