CC=g++ -Wall -O2 -DNDEBUG -fPIC
AR=ar

TARGET=./libjsoncpp.a

SOURCES =  json_reader.cpp json_value.cpp json_writer.cpp xmlwriter.cpp

OBJS=$(patsubst %.cpp,%.o, $(SOURCES))

INCLUDE=-I./

all: $(TARGET)

.cpp.o:
	$(CC) -o $@ -c $*.cpp  $(INCLUDE)

$(TARGET):$(OBJS)
	$(AR)  -r -v $(TARGET) $(OBJS)

clean:
	rm -f ./$(TARGET)
	rm -f ./*.o
