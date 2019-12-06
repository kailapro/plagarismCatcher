final: main.o
    g++ -otest main.o
main.o: main.cpp
    g++ -std=c++0x main.cpp