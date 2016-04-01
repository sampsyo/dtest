#! /bin/bash

mkdir -p dataset
cd dataset
curl -O http://nlp.stanford.edu/data/glove.6B.zip
unzip glove.6B.zip
cd ..
./convert.py
