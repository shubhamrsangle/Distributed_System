GCC = g++
LFLAGS = -std=c++11 -pthread

FILE = assign.cpp

all : $(FILE)
	$(GCC) $(LFLAGS) $(FILE)
	
clean:
	rm -rf *.txt a.out