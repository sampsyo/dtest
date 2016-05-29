.PHONY: all
all: little

little: glove.6B.100d.1k.dat glove.42B.300d.1k.dat glove.twitter.27B.25d.1k.dat
big: glove.6B.100d.dat glove.42B.300d.dat glove.twitter.27B.25d.dat

# 6B: Wikipedia 2014 + Gigaword 5
glove.6B.zip:
	curl -O http://nlp.stanford.edu/data/glove.6B.zip
glove.6B.100d.txt: glove.6B.zip
	unzip $^
	touch $@

# 42B: Common Crawl
glove.42B.300d.zip:
	curl -O http://nlp.stanford.edu/data/glove.42B.300d.zip
glove.42B.300d.txt: glove.42B.300d.zip
	unzip $^
	touch $@

# 27B: Twitter
glove.twitter.27B.zip:
	curl -O http://nlp.stanford.edu/data/glove.twitter.27B.zip
glove.twitter.27B.25d.txt: glove.twitter.27B.zip
	unzip $^
	touch $@

# Convert the data.
%.dat: %.txt
	../convert.py $^

# Truncate data files.
%.1k.txt: %.txt
	head -n1000 $^ > $@
