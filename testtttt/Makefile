C_FLAGS = -Wall
C_FILES = $(wildcard src/*.c)
H_FILES = $(wildcard inc/*.h)

O_FILES = $(patsubst src/%.c, obj/%.o, $(C_FILES))

build: bin obj $(O_FILES)
	gcc -o bin/reset  $(O_FILES) $(C_FLAGS) src/exe/reset.c
	gcc -o bin/client $(O_FILES) $(C_FLAGS) src/exe/client.c
	gcc -o bin/server $(O_FILES) $(C_FLAGS) src/exe/server.c

	rm -r obj

obj:
	mkdir obj

bin:
	mkdir bin

obj/%.o: src/%.c inc/%.h
	gcc  -c $< -o $@ $(C_FLAGS)

clean:
	rm -rf obj tmp test


.PHONY: build clean
.SILENT:
