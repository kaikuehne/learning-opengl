CCFLAGS := -Llib -Iinclude -lglfw3
OUT=

ifeq ($(OS),Windows_NT)
	CCFLAGS += -lgdi32
else
UNAME_S := $(shell uname -s)
OUT=cool
ifeq ($(UNAME_S),Darwin)
CCFLAGS +=
endif
endif

all:
	gcc src/glad.c src/main.c ${CCFLAGS} -o main
	./main.exe
