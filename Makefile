
CC = gcc
NVCC = nvcc

CFLAGS = -g -O0 -Wall
NVCCFLAGS = -g -G -O0

TARGET = enigma

C_SOURCES = $(shell find . -name '*.c')
CUDA_SOURCES = $(shell find . -name '*.cu')

C_OBJECTS = $(C_SOURCES:.c=.o)
CUDA_OBJECTS = $(CUDA_SOURCES:.cu=.o)

OBJECTS = $(C_OBJECTS) $(CUDA_OBJECTS)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(NVCC) -g $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cu
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
