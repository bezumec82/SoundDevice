#!/bin/bash
IP_ADDRESS="192.168.7.2"
PRIVATE_KEY_FILENAME=$HOME/.ssh/id_rsa #if you change this to something like bbb_rsa it will be safer when cleaning but if you don't have the config file you will have to  
                                        # ssh -i $HOME/.ssh/bbb_rsa  root@192.168.7.2
                                        # in order to log in
CONFIG_FILENAME=$HOME/.ssh/config
CLEAN=0
if [ $# -gt 0 ]; then
  while (( "$#" )); do
    if [ $1 = "-k" ]; then 
      if [[ $2 == "/"* ]]; then
        #it's full path
        PRIVATE_KEY_FILENAME=$2
      else
        #assume it's a key in .ssh
        PRIVATE_KEY_FILENAME=$HOME'/.ssh/'$2
      fi
      shift
      shift
      continue
    fi
    if [ $1 = "-i" ]; then 
      IP_ADDRESS=$2
      shift
      shift
      continue
    fi
    if [ $1 = "clean" ] ; then
      CLEAN=1
      shift
      continue
    fi
    #if we get here, then a wrong number or type of parameters have been passed 
    echo "Generates a key pair (if does not exist) and copies it over to the host, saving the settings in the /.ssh/config file
    Usage:
    -i [ipAddress] sets the host to authenticate to (default: 192.168.7.1)
    -k [filename] sets the key file to use for autentication
                  (default: ~/.ssh/id_rsa)" 
    exit 1;
  done;
fi

if [ $CLEAN -eq 1 ] ; then
  printf "Cleaning ${PRIVATE_KEY_FILENAME}, ${PRIVATE_KEY_FILENAME}.pub, ${CONFIG_FILENAME} and bbb:~/.ssh/ . Are you sure? This might prevent you from accessing other services that use the same key or config files (y/n) "
  read sure
  if [ $sure = "y" ] ; then 
    ssh root@192.168.7.2 rm -rf .ssh &&\
    rm $PRIVATE_KEY_FILENAME ${PRIVATE_KEY_FILENAME}.pub $CONFIG_FILENAME
    if [ $? -ne 0 ] ; then
      printf "ERROR: error while cleaning"
      exit 4
    fi
    printf "Cleaning succesful"
    exit 0
  fi
fi
printf Pinging the Beaglebone on $IP_ADDRESS
ping $IP_ADDRESS -w 1000 -n 1 &>/dev/null #returns 1 if ping is unsuccesful
if [ $? -ne 0 ] ; then   # $? is the return value of the last command
  echo "Error: the Beaglebone is not alive, make sure it is connected and drivers are installed (MacOs and Windows only) or try again later" ;
  exit 1
fi;
printf "...done\n"

mkdir -p $HOME/.ssh # create the ssh folder if it does not exist
printf '\nHost bbb\nHostname '$IP_ADDRESS'\nUser root\nIdentityFile '$PRIVATE_KEY_FILENAME'\n' >> $HOME/.ssh/config

printf "Generating key $PRIVATE_KEY_FILENAME if it does not exist"
ls $PRIVATE_KEY_FILENAME &>/dev/null || ssh-keygen -t rsa -f $PRIVATE_KEY_FILENAME -q -P "" # the command after || will be executed only if the previous command fails (i.e. if id_rsa does not exist)
if [ $? -ne 0 ] ; then
  printf "\nERROR: an error occurred while creating key pair $PRIVATE_KEY_FILENAME\n"
  exit 2
fi
printf "...Done\n"

printf "Type 'a' (without quotes) when prompted for a password: \n"

# StrictHostKeyChecking=no below will prevent the following message upon the first connection:
# "The authenticity of host '192.168.1.2' can't be established."
# which would require the user to type 'yes'
cat ${PRIVATE_KEY_FILENAME}.pub | (ssh -q -o StrictHostKeyChecking=no bbb 'mkdir -p .ssh; cat > .ssh/authorized_keys')
if [ $? -ne 0 ] ; then
  printf "ERROR: An error occurred while copying the public key  to the BBB\n"
  exit 3
fi
printf "SSH setup complete. You can now ssh into the beaglebone with command: ssh bbb\n"
exit 0
