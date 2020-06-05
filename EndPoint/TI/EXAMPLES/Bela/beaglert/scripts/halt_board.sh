#!/bin/bash
#
# This script halts the BeagleBone Black.

BBB_ADDRESS="root@192.168.7.2"

echo "Shutting down the BeagleBone Black..."
ssh $BBB_ADDRESS "halt"