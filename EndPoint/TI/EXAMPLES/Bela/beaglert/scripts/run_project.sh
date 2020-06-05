#!/bin/bash
#
# This script runs an already-compiled BeagleRT project on the
# BeagleBone Black.

BBB_ADDRESS="root@192.168.7.2"
BBB_PATH="~/BeagleRT"
COMMAND_ARGS=
RUN_IN_FOREGROUND=0
RUN_WITHOUT_SCREEN=0

function usage
{
    THIS_SCRIPT=`basename "$0"`
    echo "Usage: $THIS_SCRIPT [-b path-on-beaglebone] [-c command-line-args] [-fF]"

    echo "
    This script runs a previously compiled BeagleRT project on the 
    BeagleBone Black. The -b option changes the default path, which
    is otherwise $BBB_PATH. The -c option passes command-line arguments
    to the BeagleRT program; enclose the argument string in quotes.
	
    The -f argument runs the project in the foreground of the current terminal,
    within a screen session that can be detached later. The -F argument runs
	the project in the foreground of the current terminal, without screen, so
	the output can be piped to another destination."
}

OPTIND=1

while getopts "b:c:fFh" opt; do
    case $opt in
        b)            BBB_PATH=$OPTARG
                      ;;
        c)            COMMAND_ARGS=$OPTARG
                      ;;
        f)            RUN_IN_FOREGROUND=1
                      ;;
		F)            RUN_WITHOUT_SCREEN=1
		              ;;
        h|\?)         usage
                      exit 1
    esac
done

shift $((OPTIND-1))

echo "Running BeagleRT..."
if [ $RUN_WITHOUT_SCREEN -ne 0 ]
then
	ssh -t $BBB_ADDRESS "screen -X -S BeagleRT quit &>/dev/null; pkill BeagleRT ; sleep 0.5 ; $BBB_PATH/BeagleRT $COMMAND_ARGS"	
elif [ $RUN_IN_FOREGROUND -eq 0 ]
then
    ssh $BBB_ADDRESS "screen -X -S BeagleRT quit &>/dev/null; pkill BeagleRT ; sleep 0.5 ; screen -S BeagleRT -d -m $BBB_PATH/BeagleRT $COMMAND_ARGS"
else
    ssh -t $BBB_ADDRESS "screen -X -S BeagleRT quit &>/dev/null; pkill BeagleRT ; sleep 0.5 ; screen -S BeagleRT $BBB_PATH/BeagleRT $COMMAND_ARGS"
fi