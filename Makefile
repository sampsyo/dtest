TARGET := hash

$(TARGET): hash.cpp
	$(CXX) -o $@ $^

.PHONY: run
run: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)

results.json: compile.py hash.cpp distributions.json
	python3 compile.py distributions.json

hash_%: %.c hash.cpp
	$(CXX) -o $@ hash.cpp -D GENERATOR_C=\"$<\"

winner.json: results.json winner.py
	python3 winner.py

.PHONY: test
test: test.py data.txt ../apps/hash/distributions.json
	python3 $^
