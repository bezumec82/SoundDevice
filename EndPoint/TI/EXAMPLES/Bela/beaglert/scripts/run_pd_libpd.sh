#!/bin/bash
#
# This script compiles a BeagleRT project on the BeagleBone Black and
# optionally runs it. Pass a directory path in the first argument. 
# The source files in this directory are copied to the board and compiled.

BBB_ADDRESS="root@192.168.7.2"
BBB_LIBPD_EXECUTABLE_PATH="~/libpd/BeagleRT"
BBB_LIBPD_PROJECT_PATH="~/libpd/source/"
RUN_PROJECT=1
COMMAND_ARGS=
RUN_IN_FOREGROUND=0
RUN_WITHOUT_SCREEN=0

function usage
{
    THIS_SCRIPT=`basename "$0"`
    echo "Usage: $THIS_SCRIPT [-c command-line-args] [-nfF] <directory-with-source-files>"
    echo "
    This script copies a PureData project to the BeagleBone and runs it 
    using libpd. The BeagleRT-libpd executable should have first been copied
    to the $BBB_LIBPD_EXECUTABLE_PATH folder on the Beaglebone.
    The source directory should contain a file called _main.pd, which is the 
    patch that will be loaded into Pd. All the content of the folder is 
    recursively copied and the folder structure is flattened.
    If the argument -n is passed, the output will not be run after compiling.
    The -c option passes command-line arguments to the BeagleRT program; 
    enclose the argument string in quotes.
	
    The -f argument runs the project in the foreground of the current terminal,
    within a screen session that can be detached later with ctrl-A ctrl-D.
    The -F argument runs the project in the foreground of the current terminal,
    without screen, so	the output can be piped to another destination."
}

OPTIND=1

while getopts "b:c:nfFh" opt; do
    case $opt in
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

PROJECT_FILES=$($FIND_STRING)

if [[ -z $PROJECT_FILES ]]
then
    echo "Please provide a directory containing the _main.pd file and additional abstractions"
    usage
    exit
fi

# Stop BeagleRT and clean out old source files
echo "Stopping BeagleRT and removing old source files..."
ssh -t -t $BBB_ADDRESS "screen -X -S BeagleRT quit &>/dev/null;\
 pkill BeagleRT ; rm -rf $BBB_LIBPD_PROJECT_PATH/; mkdir -p $BBB_LIBPD_PROJECT_PATH; "

# Copy new source files to the board
echo "Copying new pd projects to BeagleBone..."
scp $PROJECT_FILES "$BBB_ADDRESS:$BBB_LIBPD_PROJECT_PATH"

if [ $? -ne 0 ]
then
	echo "Error while copying files"
	exit
fi

# Make new BeagleRT executable and run
if [ $RUN_PROJECT -eq 0 ]
then
    echo "Files copied. Run without \"-n\" to run the project"
else
    echo "Running project..."
	
	if [ $RUN_WITHOUT_SCREEN -ne 0 ]
	then
		ssh -t $BBB_ADDRESS "cd $BBB_LIBPD_PROJECT_PATH && $BBB_LIBPD_EXECUTABLE_PATH $COMMAND_ARGS"
	elif [ $RUN_IN_FOREGROUND -eq 0 ]
	then
	    ssh $BBB_ADDRESS "cd $BBB_LIBPD_PROJECT_PATH && screen -S BeagleRT -d -m \
         $BBB_LIBPD_EXECUTABLE_PATH $COMMAND_ARGS"
	else
	    ssh -t $BBB_ADDRESS "cd $BBB_LIBPD_PROJECT_PATH && screen -S BeagleRT $BBB_LIBPD_EXECUTABLE_PATH $COMMAND_ARGS"
	fi
fi