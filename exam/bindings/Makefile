all: python

python: stack_c_interface.o
	gcc -shared -fpic -o libstackpool.so python_interface.c stack_c_interface.o -lstdc++ -std=c11

stack_c_interface.o:
	g++ -fpic -c stack_c_interface.cpp -o stack_c_interface.o -std=c++14

clean:
	rm -f stack_c_interface.o libstackpool.so
