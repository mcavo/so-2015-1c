C_FLAGS = -Wall 
C_FILES = $(wildcard src/*.c)
H_FILES = $(wildcard inc/*.h)

O_FILES = $(patsubst src/%.c, obj/%.o, $(C_FILES))

build: bin obj $(O_FILES)
	gcc -o bin/main   $(O_FILES) $(C_FLAGS) src/exe/main.c

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
