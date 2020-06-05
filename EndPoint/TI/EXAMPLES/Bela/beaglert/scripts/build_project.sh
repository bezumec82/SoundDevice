#!/bin/bash
#
# This script compiles a BeagleRT project on the BeagleBone Black and
# optionally runs it. Pass a directory path in the first argument. 
# The source files in this directory are copied to the board and compiled.

BBB_ADDRESS="root@192.168.7.2"
BBB_PATH="~/BeagleRT"
RUN_PROJECT=1
COMMAND_ARGS=
RUN_IN_FOREGROUND=0
RUN_WITHOUT_SCREEN=0

function usage
{
    THIS_SCRIPT=`basename "$0"`
    echo "Usage: $THIS_SCRIPT [-b path-on-beaglebone] [-c command-line-args] [-nfF] <directory-with-source-files>"
    echo "
    This script copies a directory of source files to the BeagleBone, compiles
    and runs it. The BeagleRT core files should have first been copied over
    using the setup_board.sh script supplied with BeagleRT.

    The source directory should contain at least one .c, .cpp or .S file.
    If the argument -n is passed, the output will not be run after compiling.
    The argument -b will change the local path on the BeagleBone where the
    BeagleRT files are found. The -c option passes command-line arguments to
    the BeagleRT program; enclose the argument string in quotes.
	
    The -f argument runs the project in the foreground of the current terminal,
    within a screen session that can be detached later. The -F argument runs
	the project in the foreground of the current terminal, without screen, so
	the output can be piped to another destination."
}

OPTIND=1

while getopts "b:c:nfFh" opt; do
    case $opt in
        b)            BBB_PATH=$OPTARG
                      ;;
        c)            COMMAND_ARGS=$OPTARG
                      ;;
		f)            RUN_IN_FOREGROUND=1
			          ;;
		F)            RUN_WITHOUT_SCREEN=1
			  		  ;;
        n)    	      RUN_PROJECT=0
                      ;;
        h|\?)         usage
                      exit 1
    esac
done

shift $((OPTIND-1))

# Check that we have a directory containing at least one source file
# as an argument

if [ -z "$1" ]
then
    usage
    exit
fi

FIND_STRING="find $* -maxdepth 10000 -type f "

C_FILES=$($FIND_STRING -name "*.c")
CPP_FILES=$($FIND_STRING -name "*.cpp")
ASM_FILES=$($FIND_STRING -name "*.S")

if [[ -z $C_FILES ]] && [[ -z $CPP_FILES ]] && [[ -z $ASM_FILES ]]
then
    echo "Please provide a directory containing .c, .cpp or .S files."
#    echo "Usage: $THIS_SCRIPT [directory-with-source-files]"
    usage
    exit
fi

# Stop BeagleRT and clean out old source files
echo "Stopping BeagleRT and removing old source files..."
ssh -t -t $BBB_ADDRESS "screen -X -S BeagleRT quit &>/dev/null; pkill BeagleRT ; make sourceclean -C $BBB_PATH"

#concatenate arguments to form path.
BBB_SOURCE_PATH= #initially empty, will be filled with input arguments
for i in "$@" #parse input arguments
do
  if [ -d "$1" ] #check if the path is a folder
  then #if it is, include all of its files
    BBB_SOURCE_PATH+=" ${1}/* "
  else
    BBB_SOURCE_PATH+=" $1 "
  fi
  shift
  # Copy new souce files to the board
done

# Copy new source files to the board
echo "Copying new source files to BeagleBone..."
scp $BBB_SOURCE_PATH "$BBB_ADDRESS:$BBB_PATH/source/"

if [ $? -ne 0 ]
then
	echo "Error while copying files"
	exit
fi

# Make new BeagleRT executable and run
if [ $RUN_PROJECT -eq 0 ]
then
    echo "Building project..."
    ssh $BBB_ADDRESS "make all -C $BBB_PATH"
else
    echo "Building and running project..."
	
	if [ $RUN_WITHOUT_SCREEN -ne 0 ]
	then
		ssh -t $BBB_ADDRESS "cd $BBB_PATH && make all && cd source && ../BeagleRT $COMMAND_ARGS"	
	elif [ $RUN_IN_FOREGROUND -eq 0 ]
	then
	    ssh $BBB_ADDRESS "cd $BBB_PATH && make all && cd source && screen -S BeagleRT -d -m ../BeagleRT $COMMAND_ARGS"
	else
	    ssh -t $BBB_ADDRESS "cd $BBB_PATH && make all && cd source && screen -S BeagleRT ../BeagleRT $COMMAND_ARGS"
	fi
fi
