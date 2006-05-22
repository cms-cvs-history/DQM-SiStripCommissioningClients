#!/bin/bash

eval `scramv1 ru -sh`

HOSTNAME=$(echo `/bin/hostname` | sed 's/\//\\\//g')
echo "The hostname is = $HOSTNAME"

TEST_PATH=$(echo "${PWD}" | sed 's/\//\\\//g')
echo "The current directory is = $PWD"

PACKAGE="SiStripCommissioningClients"
SCRAM_ARCH="slc3_ia32_gcc323_dbg"

LIB="${LOCALRT}/lib/$SCRAM_ARCH/libDQM${PACKAGE}.so"
echo "Looking for the library $LIB"
if [ ! -f $LIB ]; then
    echo "Library not found! Looking in afs release area..."
    LIB="/afs/cern.ch/cms/Releases/CMSSW/prerelease/${CMSSW_VERSION}/lib/$SCRAM_ARCH/libDQM${PACKAGE}.so"
else 
    echo "Found library!"
fi

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

sed -e "s/_portn/1972/g" -e "s/_host/${HOSTNAME}/g" -e "s/_pwd/${TEST_PATH}/g" -e "s/_libpath/${LIBRARY}/g" .profile.xml > profile.xml
sed -e "s/_portn/1972/g" -e "s/_host/${HOSTNAME}/g" -e "s/_pwd/${TEST_PATH}/g" -e "s/_libpath/${LIBRARY}/g" .client.xml > client.xml 
sed -e "s/_portn/1972/g" -e "s/_host/${HOSTNAME}/g" -e "s/_pwd/${TEST_PATH}/g" -e "s/_libpath/${LIBRARY}/g" .startClient > startClient

chmod 755 profile.xml
chmod 755 client.xml
chmod 755 startClient



