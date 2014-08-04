CC=gcc
CFLAGS= -O2 -g -Wall
CFLAGS += -DCONFIG_SEMAPHORE
CXXFLAGS=$CFLAGS 


OBJS =queue.o

LIBS =-lpthread

TARGET := test_parcel test_queue 

all:	$(TARGET)

%.o:%.c
	$(CC) -o $@ -c $<

test_queue:test_queue.o $(OBJS)
	$(CXX) -o $@ $(OBJS) $< $(LIBS)

test_parcel:test_parcel.o $(OBJS)
	$(CXX) -o $@ $(OBJS) $< $(LIBS)

clean:
	rm -f *.o $(TARGET)
