all: EDAmain.o MDAmain.o Qmain.o 
	g++ MDAmain.o -std=c++11 -o MDAmain
	g++ EDAmain.o -std=c++11 -o EDAmain
	g++ Qmain.o -std=c++11 -o Qmain
MDAmain.o: MDAmain.cpp
	g++ -std=c++11 -c MDAmain.cpp
EDAmain.o: EDAmain.cpp
	g++ -std=c++11 -c EDAmain.cpp
Qmain.o: Qmain.cpp
	g++ -std=c++11 -c Qmain.cpp
	
clean:
	rm *.o
	rm MDAmain
	rm EDAmain
	rm Qmain
