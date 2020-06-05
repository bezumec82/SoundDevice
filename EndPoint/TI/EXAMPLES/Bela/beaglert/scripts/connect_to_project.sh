#!/bin/bash
#
# This script brings an already running BeagleRT program to the foreground 
# in the terminal, so it can be run interactively.

BBB_ADDRESS="root@192.168.7.2"

ssh -t $BBB_ADDRESS "screen -S BeagleRT -r"