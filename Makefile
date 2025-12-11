all:
	gcc src/*.c -Iinclude -lpthread -o build/hospital

clean:
	rm -f build/hospital