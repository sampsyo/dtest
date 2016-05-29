# Build the FALCONN tool.
glove:
	g++ -std=c++11 -Wall -O3 -march=native glove.cc -o glove -I ../FALCONN/src/include -I ../FALCONN/external/eigen -pthread

# Generate the configuration alternatives for dtest.
alternatives.json: gen_alternatives.py
	python3 $<

# Fetch and convert the dataset.
.PHONY: data
data:
	make -C dataset
