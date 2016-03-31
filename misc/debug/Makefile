CC = gcc
CFLAGS = -Wall -Iinclude/

target = mytest
objects = test.o libtest.o
all:$(target)

$(objects):%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $^
$(target):$(objects)
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm *.o $(target)
