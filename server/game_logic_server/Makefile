CC=g++ -Wall -O2 -DNDEBUG -fPIC
AR=ar

TARGET=./server

SOURCES = comm_func.cpp onlineLogic.cpp serverLogic.cpp main.cpp

OBJS=$(patsubst %.cpp,%.o, $(SOURCES))

INC=-I./ \
        -I../outlib/jsoncpp/

LIB = -L../outlib/jsoncpp/ -ljsoncpp

all: $(TARGET)

.cpp.o:
	$(CC) -o $@ -c $*.cpp  $(INC)

$(TARGET):$(OBJS)
	$(CC)  -o $(TARGET) $(OBJS) $(INC) $(LIB)

clean:
	rm -f ./$(TARGET)
	rm -f ./*.o
