all: sample2D

sample2D: Sample_GL3_2D.cpp glad.c
	g++ -o sample2D Sample_GL3_2D.cpp glad.c -lGL -lglfw -lGLU -lftgl -ldl -lSOIL -I /usr/local/include/freetype2 -L/usr/local/lib -lpthread -lao -lmpg123 -std=c++11
clean:
	rm sample2D
