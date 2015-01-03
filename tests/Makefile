CC	= gcc -std=c99
C_FLAGS = -Wall -g -pedantic

all:
	@echo Compiling Stringtest
	@$(CC) stringtest.c -o stringtest ../src/dstring.o

stringtest:
	rm -f stringtest
	$(CC) stringtest.c -o stringtest ../src/dstring.o

clean:
	@echo Deleting tests $< ...
	@rm -f stringtest
