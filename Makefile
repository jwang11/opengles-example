CC=gcc

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.bin)

all: $(OBJS)
	@echo "build all"

%.bin: %.c
	gcc -o $@ $< -lwayland-client -lwayland-egl -lEGL -lGLESv2

.PHONY: clean
clean:
	@rm -f $(OBJS)
