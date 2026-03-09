#!/bin/bash

apt install -y  libmodbus-dev

SOURCEFOLDER=indi-regulusfocuser
echo
echo

source buildindidriver.incl
updateModbusCmakeFolder
