CC=gcc -Wall -O2 -DNDEBUG -fPIC
AR=ar

TARGET=./client

SOURCES = client.c

OBJS=$(patsubst %.cpp,%.o, $(SOURCES))

INC=-I./ \
        -I../outlib/jsoncpp/

LIB = -L../outlib/jsoncpp/ -ljsoncpp

all: $(TARGET)

.cpp.o:
	$(CC) -o $@ -c $*.cpp  $(INC)

.c.o:
	$(CC) -o $@ -c $*.c  $(INC)

$(TARGET):$(OBJS)
	$(CC)  -o $(TARGET) $(OBJS) $(INC) $(LIB)

clean:
	rm -f ./$(TARGET)
	rm -f ./*.o
