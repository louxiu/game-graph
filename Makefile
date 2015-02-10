# FLAGS=-g -lglut -lGLEW -lm -lSOIL -lfreetype

FLAGS=-g -lGL -lglut -lGLEW -I./SOIL -I/usr/include/freetype2/ -L./SOIL -lm -lSOIL -lfreetype

SRC=$(wildcard *.cpp)

OBJS=$(SRC:.cpp=.o)

all : main

# main : $(OBJS)
# 	g++ -o $@ $^ $(FLAGS)

main : main.cpp view1_triangle.cpp view2_triangle.cpp view3_triangle.cpp view4_triangle.cpp view5_cube.cpp view6_cube.cpp util.o engine/Mesh.o engine/Render.o engine/Program.o engine/Texture.o
	g++ -o $@ $^ $(FLAGS)

%.o : %.cpp
	g++ $(FLAGS) -c -o $@ $<

view1 : view1_triangle.cpp util.o engine/Mesh.o engine/Render.o engine/Program.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view2 : view2_triangle.cpp util.o engine/Mesh.o engine/Render.o engine/Program.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view3 : view3_triangle.cpp util.o engine/Mesh.o engine/Render.o engine/Program.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view4 : view4_triangle.cpp util.o engine/Mesh.o engine/Render.o engine/Program.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view5 : view5_cube.cpp util.o engine/Mesh.o engine/Render.o engine/Program.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view6 : view6_cube.cpp util.o engine/Mesh.o engine/Render.o engine/Program.o engine/Texture.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view7 : view7_suzanne.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view8 : view8.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view9 : view9_sphere.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view10 : view10_cube.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view11 : view11.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view12 : view12.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view13 : view13.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view14 : view14.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view15 : view15.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view16 : view16_teapot.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view17 : view17_cube.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view18 : view18.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view19 : view19.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view20 : view20_dice.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

view21 : view21_portal.cpp util.o lib/Mesh.o
	g++ -o $@ $^ $(FLAGS) -DTEST_ALONE

clean :
	rm -f *.o main view1 view2 view3 view4 view5 view6 view7 view8 view9 view10
	rm -f *.o view11 view12 view13 view14 view15 view16 view17 view18
	rm -f *.o view19 view20 view21 
	rm -f engine/a.out

.PHONY: clean

# http://www.puxan.com/web/blog/HowTo-Write-Generic-Makefiles
