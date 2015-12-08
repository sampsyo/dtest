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

winner.json: results.json winner.py
	python3 winner.py

.PHONY: test
test: test.py data.txt distributions.json
	python3 $^