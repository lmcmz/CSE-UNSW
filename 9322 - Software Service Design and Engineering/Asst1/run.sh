#!/bin/bash

cd dentist
docker build -t dentist .
nohup docker run -p 5000:5000 -t dentist > output 2>&1 &

cd ../slot
docker build -t slot .
nohup docker run -p 5001:5001 -t slot > output 2>&1 &

cd ../client/client
pip3 install -r requirements.txt
python3 __init__.py