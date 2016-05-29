#! /bin/bash

mkdir -p dataset
cd dataset

# 6B: Wikipedia 2014 + Gigaword 5
curl -O http://nlp.stanford.edu/data/glove.6B.zip
unzip glove.6B.zip

# 42B: Common Crawl
curl -O http://nlp.stanford.edu/data/glove.42B.300d.zip
unzip glove.42B.300d.zip

# 27B: Twitter
curl -O http://nlp.stanford.edu/data/glove.twitter.27B.zip
unzip glove.twitter.27B.zip

cd ..
