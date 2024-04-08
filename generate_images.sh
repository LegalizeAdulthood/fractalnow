#!/bin/bash

usage(){
	cat<<EOF
generate_fractals.sh [OPTIONS] [FRACTAL2D_OPTIONS]
Run fractal2D on several fractal and rendering files.

OPTIONS :
-h                : Print this help
-n                : Dry-run mode (print commands without executing them)
-c <fractal_dir>  : Specify directory of fractal configuration files
-r <render_dir>   : Specify directory of fractal rendering files
-o <output_dir>   : Specify output directory for image files
FRACTAL2D_OPTIONS (see fractal2D help) :
-d 
-q 
-v 
-x <width>
-y <height>
-a <aa_method>
-s <aa_size>
-j <nb_threads>
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
EXE="./bin/fractal2D"

#${ARGS};

function error {
	echo $1 >> /dev/stderr;

	if [ ! -z $2 ]; then
		exit $2;
	else
		exit 1;
	fi;
}

#ARGS='eval set -- '`getopts ':hc:r:o:qvdnx:y:a:s:j:' "$@"`;
#while [ $# -ne 0 ] ; do
while getopts ":hc:r:o:qvdnx:y:a:s:j:" opt; do
	case "$opt" in
		'h')
			usage;
			exit 1;
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
		'r')
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
		\?)
			echo "Invalid option on command line.";
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

fractal_files=`eval ls \"${FRACTAL_DIR}\" --quoting-style=c -1 | grep ^\""${FRACTAL_FILE_PREFIX}" | sort`;

IFS=$'\n';
for i in ${fractal_files} ; do
	fractal_file=`eval echo -e "${i}"`;
	fractal_file=`basename "${fractal_file}"`;
	fractal_file_suffix=`eval echo \"${fractal_file}\" | sed "s/.*${FRACTAL_FILE_PREFIX}\(.*\)/\1/"`
	current_render_prefix="${RENDER_FILE_PREFIX}${fractal_file_suffix}"
	#render_files=`eval find \"${RENDER_DIR}\" -mindepth 1 -maxdepth 1 -type f -iname \"${current_render_prefix}*\" -execdir ls --quoting-style=c {} '\;' | sort`;
	render_files=`eval ls \"${RENDER_DIR}\" --quoting-style=c | grep ^\""${current_render_prefix}" | sort`;
	for j in ${render_files} ; do
		render_file=`eval echo -e "$j"`;
		render_file=`basename "${render_file}"`;
		cmd \"${EXE}\" -f \"${FRACTAL_DIR}/${fractal_file}\" -r \"${RENDER_DIR}/${render_file}\" -o \"${IMGS_DIR}/${fractal_file}_${render_file}${IMG_SUFFIX}\" ${FLAGS};
		if [ $? -ne 0 ]; then
			error "Error occured when running executable. Interrupting."
		fi;
		echo;
	done;
done;

