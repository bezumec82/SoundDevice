#!/bin/bash
#
# This script copies the core BeagleRT files to the BeagleBone Black
# in preparation for building projects. It will remove any existing
# BeagleRT directory before copying the files over

BBB_ADDRESS="root@192.168.7.2"
BBB_PATH="~/BeagleRT"

function usage
{
    THIS_SCRIPT=`basename "$0"`
    echo "Usage: $THIS_SCRIPT [-b path-on-beaglebone]"

    echo "
    This script copies the core BeagleRT files to the BeagleBone, REMOVING
    any previous files found at that location. This should be done before
    running any of the other build scripts in this directory. The -b option
    changes the default path, which is otherwise $BBB_PATH."
}

OPTIND=1

while getopts "b:h" opt; do
    case $opt in
        b)            BBB_PATH=$OPTARG
                      ;;
        h|\?)         usage
                      exit 1
    esac
done

echo "Copying BeagleRT core files to $BBB_PATH"

shift $((OPTIND-1))

# Find location of this script so we can locate the rest of the files
SCRIPTPATH=$(readlink -f "$0")
SCRIPTDIR=$(dirname "$SCRIPTPATH")

read -p "Warning: this script will DELETE any existing BeagleRT files from your BeagleBone! Continue? (y/N)" -r
echo
if [[ $REPLY = [yY]  ]]
then
# Stop BeagleRT if running and remove all files
  echo "Stopping BeagleRT and removing old files." 
  ssh $BBB_ADDRESS "screen -X -S BeagleRT quit &>/dev/null; pkill BeagleRT; sleep 0.5 ; rm -rf $BBB_PATH ; mkdir $BBB_PATH"

# Copy relevant files to BeagleBone Black
  echo "Copying new files to BeagleBone..."
  scp -r $SCRIPTDIR/../core $SCRIPTDIR/../include $SCRIPTDIR/../Makefile $SCRIPTDIR/../libNE10.a $SCRIPTDIR/../libprussdrv.a $BBB_ADDRESS:$BBB_PATH &&\
  scp $SCRIPTDIR/../libpd.so $BBB_ADDRESS:/usr/lib
  if [ $? -ne 0 ]
  then 
	  echo "Error while copying files"
	  exit
  fi
# Make remaining directories needed for building
  echo "Creating directory structure on BeagleBone..."
  ssh $BBB_ADDRESS "mkdir -p $BBB_PATH/source ; mkdir -p $BBB_PATH/build ; mkdir -p $BBB_PATH/build/core ; mkdir -p $BBB_PATH/build/source" &&\
  echo "Done."
else
  echo "Aborting..."
fi

