CC=gcc
CFLAGS= -O2 -g -Wall
CFLAGS += -DCONFIG_SEMAPHORE
CXXFLAGS=$CFLAGS 


OBJS =		queue.o main.o

LIBS =		-lpthread

TARGET =	testqueue

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
