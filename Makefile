TARGET := hash

$(TARGET): hash.cpp
	$(CXX) -o $@ $^

.PHONY: run
run: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) results.json winner.json temp.txt

results.json: compile.py hash.cpp distributions.json alternatives.json
	python3 compile.py distributions.json alternatives.json

winner.json: results.json winner.py
	python3 winner.py

.PHONY: test
test: test.py zipcodes.txt distributions.json
	python3 $^

.PHONY: eval
eval: eval.py alternatives.json zipcodes.txt
	python3 $^
