src:
	g++ src/*.cpp src/Player/*.cpp  -o main -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -std=c++20


# $(find src -type f -iregex ".*\.cpp")
