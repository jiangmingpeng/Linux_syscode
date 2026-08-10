libmystdio.so:main.o mystdio.o mystring.o
	gcc -o $@ $^ -shared

%.o:%.c
	gcc -fPIC -c $<


.PHONY:output
output:
	mkdir -p ./mylib
	mkdir -p ./mylib/myinclude
	mkdir -p ./mylib/lib
	cp *.h ./mylib/myinclude
	cp *.so ./mylib/lib
.PHONY:clean
clean:
	rm -rf main *.o *.so mylib
