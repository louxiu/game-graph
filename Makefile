FLAGS=-g -lglut -lGLEW -lm -lSOIL -lfreetype

SRC=$(wildcard *.cpp)

OBJS=$(SRC:.cpp=.o)

all : main

main : $(OBJS)
	g++ -o $@ $^ $(FLAGS)

%.o : %.cpp
	g++ $(FLAGS) -c -o $@ $<

clean:
	rm -f *.o main

.PHONY: clean

# http://www.puxan.com/web/blog/HowTo-Write-Generic-Makefiles
