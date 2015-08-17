TARGET := hash

$(TARGET): hash.c
	$(CC) -o $@ $^

.PHONY: run
run: $(TARGET)
	./$(TARGET)
