#!/bin/bash

usage(){
	cat<<EOF
generate_fractals.sh [OPTIONS] [FRACTAL2D_OPTIONS]
Run fractal2D on several fractal and rendering files.

OPTIONS :
-h | --help 	: Print this help
-c | --config	: Specify directory of fractal configuration files
-r | --render 	: Specify directory of fractal rendering files
-o | --ouput	: Specify output directory for image files
-n | --dry-run 	: Dry-run mode (print commands without executing them)
FRACTAL2D_OPTIONS (see fractal2D help) :
-x | --width
-y | --height
-d | --debug
-q | --quiet
-v | --verbose
-a | --anti-aliasing
-s | --aa-size
-j | --nb-threads
EOF
}

QUIET=0;
VERBOSE=0;
DRY_RUN=0;
DEBUG=0;
FRACTAL_DIR="configs"
RENDER_DIR="configs"
IMGS_DIR="imgs"
FRACTAL_FILE_PREFIX="fractal"
RENDER_FILE_PREFIX="render"
EXE="./fractal2D"

ARGS='eval set -- '`getopt -s bash -l 'help,config:,render:,ouput:,quiet,verbose,debug,dry-run,width:,height:,anti-aliasing:,aa-size:,nb-threads:' 'hc:r:o:qvdnx:y:a:s:j:' "$@"`;

if [ $? -ne 0 ]; then
	usage;
	exit 1;
fi;

${ARGS};

function error {
	echo $1 >> /dev/stderr;

	if [ ! -z $2 ]; then
		exit $2;
	else
		exit 1;
	fi;
}

while [ $# -ne 0 ] ; do
	case "$1" in
		'-h'|'--help')
			usage;
			exit 1;
		;;
		'-c'|'--config')
			shift;
			FRACTAL_DIR="$1";
		;;
		'-r'|'--render')
			shift;
			RENDER_DIR="$1";
		;;
		'-o'|'--ouput')
			shift;
			IMGS_DIR="$1";
		;;
		'-q'|'--quiet')
			QUIET=1;
		;;
		'-v'|'--verbose')
			VERBOSE=1;
		;;
		'-d'|'--debug')
			DEBUG=1;
		;;
		'-r'|'--dry-run')
			DRY_RUN=1;
		;;
		'-x'|'--width')
			shift;
			if eval [ "$1" -eq "$1" 2>/dev/null ]; then
				WIDTH=`eval echo "$1"`;
			else
				error 'Width is not a number.';
			fi;
		;;
		'-y'|'--height')
			shift;
			if eval [ "$1" -eq "$1" 2>/dev/null ]; then
				HEIGHT=`eval echo "$1"`;
			else
				error 'Height is not a number.';
			fi;
		;;
		'-a'|'anti-aliasing')
			shift;
			AAM="$1";
		;;
		'-s'|'aa-size')
			shift;
			if eval [ "$1" -eq "$1" 2>/dev/null ]; then
				AAM_SIZE=`eval echo "$1"`;
			else
				error 'AAM_Size is not a number.';
			fi;
		;;
		'-j'|'--nb-threads')
			shift;
			if eval [ "$1" -eq "$1" 2>/dev/null ]; then
				NB_THREADS=`eval echo "$1"`;
			else
				error 'Number of threads is not a number.';
			fi;
		;;
	esac;

	shift;
done;

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
	cmd mkdir "${IMGS_DIR}";
fi;

#Set fractal2D flags
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
if [ ! -z ${AAM} ]; then
	FLAGS="$FLAGS -a ${AAM}";
	IMG_SUFFIX="${IMG_SUFFIX}_a${AAM}";
fi;
if [ ! -z ${AAM_SIZE} ]; then
	FLAGS="$FLAGS -s ${AAM_SIZE}";
	IMG_SUFFIX="${IMG_SUFFIX}_s${AAM_SIZE}";
fi;
IMG_SUFFIX="${IMG_SUFFIX}.ppm";

fractal_files=`eval find \"${FRACTAL_DIR}\" -mindepth 1 -maxdepth 1 -type f -iname \"${FRACTAL_FILE_PREFIX}*\" -execdir ls --quoting-style=c {} '\;' | sort`;

IFS=$'\n';
for i in ${fractal_files} ; do
	fractal_file=`eval echo -e "${i}"`;
	fractal_file=`basename "${fractal_file}"`;
	fractal_file_suffix=`eval echo \"${fractal_file}\" | sed "s/.*${FRACTAL_FILE_PREFIX}\(.*\)/\1/"`
	current_render_prefix="${RENDER_FILE_PREFIX}${fractal_file_suffix}"
	render_files=`eval find \"${RENDER_DIR}\" -mindepth 1 -maxdepth 1 -type f -iname \"${current_render_prefix}*\" -execdir ls --quoting-style=c {} '\;' | sort`;
	for j in ${render_files} ; do
		render_file=`eval echo -e "$j"`;
		render_file=`basename "${render_file}"`;
		cmd \"${EXE}\" -c \"${FRACTAL_DIR}/${fractal_file}\" -r \"${RENDER_DIR}/${render_file}\" -o \"${IMGS_DIR}/${fractal_file}_${render_file}${IMG_SUFFIX}\" ${FLAGS};
		echo;
		if [ $? -ne 0 ]; then
			error "Error occured when running executable. Interrupting."
		fi;
	done;
done;

