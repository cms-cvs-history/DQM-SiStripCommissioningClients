#!/bin/bash

eval `/afs/cern.ch/cms/sw/bin/scramv1 runtime -sh`

HOST=$(echo `/bin/hostname` | sed 's/\//\\\//g')
echo "The hostname is = $HOST"

PWD=$(echo "${PWD}" | sed 's/\//\\\//g')
echo "The current directory is = $PWD"

APPL="SiStripCommissioningDbClient"
PACKAGE="DQMSiStripCommissioningDbClients"
SCRAM_ARCH="slc3_ia32_gcc323"

LIB="${LOCALRT}/lib/$SCRAM_ARCH/lib${PACKAGE}.so"
echo "Looking for the library $LIB"
if [ ! -f $LIB ]; then
    echo "Library not found! Looking in afs release area..."
    LIB="/afs/cern.ch/cms/Releases/CMSSW/prerelease/${CMSSW_VERSION}/lib/$SCRAM_ARCH/lib${PACKAGE}.so"
else 
    echo "Found library!"
fi

DIR=$CMSSW_BASE/src/DQM/SiStripCommissioningClients/test

LIBRARY=$(echo "$LIB" | sed 's/\//\\\//g')
echo $LIBRARY

if [ -e profile.xml ]; then
    rm profile.xml
fi 
if [ -e client.xml ]; then
    rm client.xml
fi
if [ -e startClient ]; then
    rm startClient
fi

cp $DIR/style.css .
cp $DIR/WebLib.js .
sed -e "s/_port_/1977/g" -e "s/_host_/${HOST}/g" -e "s/_pwd_/${PWD}/g" -e "s/_lib_/${LIBRARY}/g" $DIR/.profile.xml > profile.xml
sed -e "s/_port_/1977/g" -e "s/_host_/${HOST}/g" -e "s/_pwd_/${PWD}/g" -e "s/_lib_/${LIBRARY}/g" -e "s/_appl_/${APPL}/g" $DIR/.client.xml > client.xml 
sed -e "s/_port_/1977/g" -e "s/_host_/${HOST}/g" -e "s/_pwd_/${PWD}/g" -e "s/_lib_/${LIBRARY}/g" $DIR/.startClient > startClient

chmod 755 profile.xml
chmod 755 client.xml
chmod 755 startClient


