all:
	g++ -std=c++17 src/*.cpp -o main  -g -lglfw -lpthread -ldl -lXrandr -lGLEW -lGL -DGL_SILENCE_DEPRECATION -DGLM_ENABLE_EXPERIMENTAL -I./include

