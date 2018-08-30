
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.bin)

all: $(OBJS)
	@echo "build all"

%.bin: %.cpp
	g++ -o $@ $< -lX11 -lEGL -lGLESv2

.PHONY: clean
clean:
	@rm -f $(OBJS)
