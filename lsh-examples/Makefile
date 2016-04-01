all:
	g++ -std=c++11 -Wall -O3 -march=native glove.cc -o glove -I ../FALCONN/src/include -I ../FALCONN/external/eigen -pthread

alternatives.json: gen_alternatives.py
	python3 $<
