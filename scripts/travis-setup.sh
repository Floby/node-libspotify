#!/bin/bash

mkdir spotify_key
echo "exports.login = '$LOGIN'" > spotify_key/passwd.js
echo "exports.password = '$PASSWORD'" >> spotify_key/passwd.js
pwd
cd spotify_key
pwd
wget $SPKEY
