#Makefile

TARGET=pa1
CXX=gcc
OBJECTS=main.o strings.o find.o

$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)

main.o: main.c
	$(CXX) -c $^

strings.o: strings.c
	$(CXX) -c $^

find.o: find.c
	$(CXX) -c $^

clean:
	rm $(OBJECTS) $(TARGET)	
