all: ToyDatabase.o 
	g++ ToyDatabase.o -o Toy
	rm ./*.o -v

ToyDatabase.o:
	g++ -g -c src/ToyDatabase.cpp 
