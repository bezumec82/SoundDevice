#!/bin/bash

# shell script for automatic uploading/compiling of pd patch onto bbb
# Christian Heinrichs 2015
#
# example usage: sh upload-and-compile.sh -f bwg-tests -q -e

trap "{ echo "";exit 0; }" SIGINT SIGTERM

workingdir=".."
verbose="0"
render="0"
pdpath=""
WATCH="0"
FORCE="0"
#make sure the paths have the trailing / . 
projectpath="../projects/heavy/hvtemp/"
BBB_PATH="~/BeagleRT/"
BBB_ADDRESS="root@192.168.7.2"
COMMAND_ARGS=
RUN_PROJECT=1
RUN_IN_FOREGROUND=0
RUN_WITHOUT_SCREEN=1
BELA_PYTHON27=

if [ -z "$BELA_PYTHON27" ]; then
    for PY in python python2.7 ; do
        python --version 2>&1 | grep "2\.7" &> /dev/null
        if [ $? -eq 0 ]; then
            BELA_PYTHON27=$PY
            break;
        fi;
    done;
fi;

if [ -z "$BELA_PYTHON27" ]; then
    echo "It looks like you might not have python2.7 installed. If you do, please specify the path
    to your python2.7 executable in the environmental variable $BELA_PYTHON27"
    exit 1;
fi;


function usage
{
printf "\nUSAGE: build_pd.sh [[-i input folder containing _main.pd file ]\
 [-o output folder for new heavy project .c files (default ../projects/heavy/hvtemp)]\
 [-b bbb path to copy to (default ~/BeagleRT)] | [-h] | [-f|--force] | [-w|--watch]\n"
printf "\nexample: build_pd.sh -i ../projects/heavy/pd/hello-world -o ../projects/heavy/hello-world\n"
echo "If --watch is selected, the script will check every 1s for any file that is modified in the source folder, which triggers\
the building process and runs the process.
If --screen is selected, the prompt returns to the user after launching BeagleRT in a screen on the target device.
If --screen and --watch are combined, while the process is running in the screen, modifications to the source files will \
still trigger a new build."
}

while [ "$1" != "" ]; do
    case $1 in
        -b | --bbb )            shift
                                BBB_PATH=$1
                                ;;
        -f | --force)           FORCE="1"
                                ;;
        -i | --input )          shift
                                pdpath=$1
                                ;;
        -o | --output )         shift
                                projectpath=$1
                                ;;
        -v | --verbose )        verbose=1
                                ;;
        -r | --render )         shift
                                render=1
                                ;;
        -s | --screen )         RUN_WITHOUT_SCREEN="0"
                                ;;
        -w | --watch )          WATCH=1
                                ;;
        -h | --help )           usage
                                exit
                                ;;
        * )                     usage
                                exit 1
    esac
    shift
done

function hasNotChanged(){
    if [ $WATCH -eq 0 ];
    then
        echo "Files in the source folder did not change since the last build, use --force to force recompiling";
        exit 0;
    fi;
}

function checkChanged(){
    PD_BACKUP_PATH="hvresources/patch_back/"
    if [ $FORCE -eq 1 ] && [ $WATCH -eq 0 ];
    then
        rm -rf $PD_BACKUP_PATH;
        return;
    fi;
    mkdir -p $PD_BACKUP_PATH;
    # count files that have changed
    HAS_CHANGED=$(rsync -nac --out-format="%f" "$pdpath" $PD_BACKUP_PATH | grep -v "\/\.$"| wc -l);
    if [ $HAS_CHANGED -eq 0 ] && [ $FORCE -eq 0 ];
    then
        hasNotChanged;
        return 1;
    fi;
     #if we are here and $FORCE==1, it means that $WATCH==1
     # so let's make sure only the first run get forced:
    FORCE=0;
    echo "Files have changed"
    # otherwise back up the files that have changed
    rsync -vac "$pdpath" $PD_BACKUP_PATH

    return 0;
}

function checkUploadBuildRun(){
    checkChanged || return # exits if source files have not changed

    # remove old static files to avoid obsolete errors
    # use -rf to prevent warnings in case they do not exist
    rm -rf "$projectpath"/Hv* "$projectpath"/Message* "$projectpath"/Control* "$projectpath"/Signal* &>/dev/null

    # invoke the online compiler
    "$BELA_PYTHON27" hvresources/uploader.py "$pdpath"/ -n bbb -g c -o "$projectpath";
    if [ $? -ne 0 ]; then
    #echo "ERROR: an error occurred while executing the uploader.py script"
    echo "error"
    exit 1
    fi;

    echo "";
    #echo "*|*|* Successfully uploaded and converted pd patch into super-fast optimized C code. Brought to you by Heavy! *|*|*";
    echo "";

    # check how to copy/sync render.cpp file...
    if [ $render -eq 0 ]; then
    cp "hvresources/render.cpp" $projectpath/;
    fi;

    cp "hvresources/HvUtils.h" $projectpath/;

    echo "updating files on board..."

    rsync -c -rv --exclude 'HvContext*' "$projectpath"/ "$BBB_ADDRESS":"$BBB_PATH"/source;
    # rsync -c -rv "$projectpath"/ "$BBB_ADDRESS":"$BBB_PATH"/source;

    # for whatever reason these big files used to hang when transferring with rsync
    scp "$projectpath"/HvContext* "$BBB_ADDRESS":"$BBB_PATH"/source;

    if [ $? -ne 0 ]; then
    echo "";
    echo ":( :( :( ERROR: while synchronizing files with the BBB. Is the board connected and the correct SD card inserted? :( :( :(";
    echo "";
    exit 1;
    fi;
    # exit
    #produce a list of files which content has changed (not just the date)
    #TODO: could be made faster (perhaps) by backing up the folder locally instead of bbb
    # UPDATED_FILES=`rsync -naic --log-format="%f" "$projectpath" "$BBB_PATH"/source | grep -v "\.$"`
    # echo "UPDATEDFILES : $UPDATED_FILES"
    # exit 2
    # remove old executable and heavy context .o/.d files
    ssh $BBB_ADDRESS "rm -rf $BBB_PATH/BeagleRT $BBB_PATH/build/source/HvContext_bbb.d $BBB_PATH/build/source/HvContext_bbb.o $BBB_PATH/build/source/render.o $BBB_PATH/build/source/render.d";
    SCREEN_NAME=BeagleRT
    KILL_RUNNING_PROCESS="bash -c 'kill -s 2 \`pidof BeagleRT\` 2>/dev/null; screen -r $SCREEN_NAME -X quit 2>/dev/null; sleep 0.5; exit 0'" #always returns true
    # Make new BeagleRT executable and run
    # It does not look very nice that we type the same things over and over
    # but that is because each line is an ssh session in its own right
    if [ $RUN_PROJECT -eq 0 ]
    then
        echo "Building project..."
        ssh $BBB_ADDRESS "make all -C $BBB_PATH"
    else
        echo "Building and running project..."
        if [ $RUN_WITHOUT_SCREEN -eq 1 ]
        then
            ssh -t $BBB_ADDRESS "make all -C $BBB_PATH && $KILL_RUNNING_PROCESS && $BBB_PATH/BeagleRT $COMMAND_ARGS" 
        elif [ $RUN_IN_FOREGROUND -eq 1 ]
        then
            # Run in screen without detaching
            ssh -t $BBB_ADDRESS "make all -C $BBB_PATH && $KILL_RUNNING_PROCESS && screen -S $SCREEN_NAME $BBB_PATH/BeagleRT $COMMAND_ARGS"
        else
            # Run in screen and detach
            ssh $BBB_ADDRESS "make all -C $BBB_PATH && $KILL_RUNNING_PROCESS && screen -dmS $SCREEN_NAME $BBB_PATH/BeagleRT $COMMAND_ARGS"
        fi
    fi
} #checkUploadBuildRun

checkUploadBuildRun

if [ $WATCH -ne 0 ]; then
    WAIT_STRING="\rWaiting for changes in $pdpath"
    while true; do
        printf "$WAIT_STRING   "
        sleep 0.3
        printf "$WAIT_STRING.  "
        sleep 0.3
        printf "$WAIT_STRING.. "
        sleep 0.3
        printf "$WAIT_STRING..."
        sleep 0.3
        checkUploadBuildRun
    done;
fi;
#ssh -t root@192.168.7.2 "kill -s 2 \`pidof heavy_template\` 2>/dev/null; sleep 0.5; rm -f ~/$filename_bbb/Release/source/heavy/HvContext_bbb.? ~/$filename_bbb/Release/heavy_template && make all -C ~/$filename_bbb/Release" &>/dev/null
