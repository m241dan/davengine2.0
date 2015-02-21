CC      = gcc -std=c99
C_FLAGS = -Wall -g3 -ggdb3 -pedantic `mysql_config --cflags`
L_FLAGS = -lz -lpthread -lcrypt -llua -ldl -lm `mysql_config --libs`

O_FILES = socket.o io.o strings.o utils.o interpret.o help.o  \
	  mccp.o save.o event.o event-handler.o \
	  list.o stack.o hash.o manager.o lua_utils.o sqldb.o

all: $(O_FILES)
	rm -f SocketMud
	$(CC) -o SocketMud $(O_FILES) $(L_FLAGS)

.c.o: all
	@echo [`date +%T`] Compiling $< ...
	@$(CC) -c $(C_FLAGS) $<

clean:
	@echo Cleaning code $< ...
	@rm -f *.o
	@rm -f SocketMud
	@rm -f *.*~
