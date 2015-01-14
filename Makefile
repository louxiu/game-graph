# FLAGS=-g -lglut -lGLEW -lm -lSOIL -lfreetype

FLAGS=-g -lGL -lglut -lGLEW -I./SOIL -I/usr/include/freetype2/ -L./SOIL -lm -lSOIL -lfreetype

SRC=$(wildcard *.cpp)

OBJS=$(SRC:.cpp=.o)

all : main

main : $(OBJS)
	g++ -o $@ $^ $(FLAGS)

%.o : %.cpp
	g++ $(FLAGS) -c -o $@ $<

clean :
	rm -f *.o main

view1 : view1_triangle.cpp view1_triangle.h util.h util.cpp global.h Mesh.h Mesh.cpp
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view2 : view2_triangle.cpp view2_triangle.h util.h util.cpp global.h Mesh.h Mesh.cpp
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view3 : view3_triangle.cpp view3_triangle.h util.h util.cpp global.h Mesh.h Mesh.cpp
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view4 : view4_triangle.cpp view4_triangle.h util.h util.cpp global.h Mesh.h Mesh.cpp
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view5 : view5_cube.cpp view5_cube.h util.h util.cpp global.h Mesh.h Mesh.cpp
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view21 : view21_portal.cpp view21_portal.h util.h util.cpp global.h Mesh.h Mesh.cpp
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

.PHONY: clean

# http://www.puxan.com/web/blog/HowTo-Write-Generic-Makefiles
