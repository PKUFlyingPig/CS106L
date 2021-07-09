CC = gcc
CFLAGS = -c
OBJ_FILES = main.o error.o wikiscrapper.o
.PHONY : clean

main : $(OBJ_FILES)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -o $@ $<

clean : 
	rm *.o main


