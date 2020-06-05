#!/bin/bash
#
# This script stops the BeagleRT program running on the BeagleBone.

BBB_ADDRESS="root@192.168.7.2"

echo "Stopping BeagleRT..."

# The first command should be sufficient to stop any BeagleRT run with
# these scripts; the second will catch any leftovers run other ways
ssh $BBB_ADDRESS "screen -X -S BeagleRT quit ; pkill BeagleRT"