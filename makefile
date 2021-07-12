.PHONY: src testing

src:
	g++ src/*.cpp src/Player/*.cpp src/Script/*.cpp  -o main -lboost_system -lpthread -llua -ldl -std=c++17


# $(find src -type f -iregex ".*\.cpp")
