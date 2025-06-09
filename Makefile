all: grafo.o mymath.o nodes.o train.o
	g++ grafo.o mymath.o nodes.o train.o main.cpp -o main

graph.o: grafo.h
	g++ -c grafo.cpp

mymath.o: mymath.h
	g++ -c mymath.cpp

nodes.o: nodes.h
	g++ -c nodes.cpp

train.o: train.h
	g++ -c train.cpp

clear:
	rm -rf *.o main vecs*.dat vecs*.png

grafico2d:
	python3 plotVec2d.py