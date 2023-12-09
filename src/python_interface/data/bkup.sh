#!/bin/sh
# Script to backup production database to JSON files.

. ./common.sh

for collection in ${IDECollections[@]}; do
    echo "Backing up $collection"
    $EXP --collection=$collection --db=$DB --out=$BKUP_DIR/$collection.json --uri="$CONNECT_STR"
    #$EXP --collection=$collection --db=$DB --out=$BKUP_DIR/$collection.json $CONNECT_STR --username $USER --password $MONGO_PASSWD
done

# Add changes to git
git add $BKUP_DIR/*.json

# Commit the changes
git commit -m "Mongo DB backup"

# Pull the latest changes from the remote repository
# Make sure you are merging changes into the correct branch
git pull origin main

# Push the changes to the remote repository
# Again, make sure to use the correct branch name
git push origin main