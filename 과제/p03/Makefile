#Makefile

TARGET=p2.out
CXX=gcc
OBJECTS=main.o plus.o minus.o

$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)

main.o: main.c
	$(CXX) -c $^

plus.o: plus.c
	gcc -c plus.c

minus.o: minus.c
	gcc -c minus.c

clean:
	rm $(OBJECTS) $(TARGET)	
