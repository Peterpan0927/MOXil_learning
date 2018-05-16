#! /bin/bash
USER=$1
ID=$2
FULLNAME=$3

dscl . -create /Users/$USER

dscl . -create /Users/$USER UserShell /bin/zsh

dscl . -create /Users/$USER RealName "$FULLNAME"
dscl . -create /Users/$USER UniqueID $ID

dscl . -create /Users/$USER PrimaryGroupID 61

dscl. -create /Users/$USER NFSHomeDirectory /Users/$USER

mkdir /Users/$USER
chown $USER /Users/$USER

dscl . -passwd /Users/$USER "yourownpasswd"

dscl . -append /Groups/admin GroupMembership $USER
