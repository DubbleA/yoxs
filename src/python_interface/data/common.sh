#!/bin/sh
# Some common shell stuff.

echo "Importing from common.sh"

DB=monkeyIDE
#USER=gcallah
CONNECT_STR="mongodb+srv://yoxs_admin:9K668SonHpk9Oc2H@cluster0.jowbltk.mongodb.net/"
if [ -z $DATA_DIR ]
then
    DATA_DIR=/Users/nlmb/Documents/yoxs/src/python_interface/data
fi

BKUP_DIR=$DATA_DIR/bkup
EXP=/opt/homebrew/bin/mongoexport
IMP=/opt/homebrew/bin/mongoimport

# if [ -z $MONGO_PASSWD ]
# then
#     echo "You must set MONGO_PASSWD in your env before running this script."
#     exit 1
# fi

declare -a IDECollections=("samples") # could have like ("samples" "games") etc