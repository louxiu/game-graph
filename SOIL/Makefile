SRC=$(wildcard *.c)

OBJS=$(SRC:.c=.o)

libSOIL.a : $(OBJS)
	ar -rcs $@ $^  

%.o : %.cpp
	g++ -c -o $@ $< -g 

clean:
	rm -f *.o

.PHONY: all clean
