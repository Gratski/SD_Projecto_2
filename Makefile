CC = gcc

O_FILES = object
C_FILES = source
H_FILES = include
CFLAGS = -g -Wall -I$(H_FILES)

all: test_data test_entry test_list

clean:
	rm -f object/*.o && rm -f test_data && rm -f test_entry && rm -f test_list



test_data: $(O_FILES)/test_data.o $(O_FILES)/data.o
	$(CC) $(CFLAGS) -o test_data $(O_FILES)/test_data.o $(O_FILES)/data.o

test_entry: $(O_FILES)/test_entry.o $(O_FILES)/data.o $(O_FILES)/entry.o
	$(CC) $(CFLAGS) -o test_entry $(O_FILES)/test_entry.o $(O_FILES)/data.o $(O_FILES)/entry.o

test_list: $(O_FILES)/test_list.o $(O_FILES)/data.o $(O_FILES)/entry.o $(O_FILES)/list.o
	$(CC) $(CFLAGS) -o test_list $(O_FILES)/test_list.o $(O_FILES)/data.o $(O_FILES)/entry.o $(O_FILES)/list.o


# Testes
$(O_FILES)/test_data.o: $(C_FILES)/test_data.c $(H_FILES)/data.h
	$(CC) $(CFLAGS) -c $(C_FILES)/test_data.c -o $(O_FILES)/test_data.o

$(O_FILES)/test_entry.o: $(C_FILES)/test_entry.c $(H_FILES)/data.h $(H_FILES)/entry.h
	$(CC) $(CFLAGS) -c $(C_FILES)/test_entry.c -o $(O_FILES)/test_entry.o

$(O_FILES)/test_list.o: $(C_FILES)/test_list.c $(H_FILES)/list-private.h $(H_FILES)/list.h
	$(CC) $(CFLAGS) -c $(C_FILES)/test_list.c -o $(O_FILES)/test_list.o


# Structs
$(O_FILES)/data.o: $(C_FILES)/data.c $(H_FILES)/data.h
	$(CC) $(CFLAGS) -c $(C_FILES)/data.c -o $(O_FILES)/data.o

$(O_FILES)/entry.o: $(C_FILES)/entry.c $(H_FILES)/data.h $(H_FILES)/entry.h
	$(CC) $(CFLAGS) -c $(C_FILES)/entry.c -o $(O_FILES)/entry.o

$(O_FILES)/list.o: $(C_FILES)/list.c $(H_FILES)/list-private.h $(H_FILES)/list.h
	$(CC) $(CFLAGS) -c $(C_FILES)/list.c -o $(O_FILES)/list.o
