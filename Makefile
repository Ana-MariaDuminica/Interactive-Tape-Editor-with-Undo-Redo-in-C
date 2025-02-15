CC=gcc
DEPS=tema1.c
EXE=tema1

build: $(DEPS)
	$(CC) -o $(EXE) $^

.PHONY: clean

clean:
	rm -f $(EXE)
	> $(EXE).in
	> $(EXE).out