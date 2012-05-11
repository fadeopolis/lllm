
OBJ_DIR=build/src/


all:
	clear2
	clang++ -std=gnu++11 test.cpp build/src/util/libutil.a lib/libjit.a -Ilib -Iinclude -lpthread -lm -o test
	./test
