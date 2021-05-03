.PHONY: src testing

src:
	g++ src/*.cpp src/Player/*.cpp src/Script/*.cpp  -o main -lsfml-system -lsfml-network -llua -ldl -std=c++20


# $(find src -type f -iregex ".*\.cpp")
