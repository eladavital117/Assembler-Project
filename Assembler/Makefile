CC = gcc
CFLAGS = -ansi -Wall -pedantic
GLOBAL_DEPS = globals.h
EXE_DEPS = main.o utils.o valid.o


main: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

main.o: main.c $(GLOBAL_DEPS)
	$(CC) -c main.c $(CFLAGS) -o $@

utils.o: utils.c $(GLOBAL_DEPS)
	$(CC) -c utils.c $(CFLAGS) -o $@

valid.o: valid.c $(GLOBAL_DEPS)
	$(CC) -c valid.c $(CFLAGS) -o $@

