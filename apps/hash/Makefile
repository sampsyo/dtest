TARGET := hash

$(TARGET): hash.cpp
	$(CXX) -o $@ $^

.PHONY: run
run: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)
