OBJS := simple_triangle uniform_var translation rotation

all: $(OBJS)
	@echo 

simple_triangle: common.c t4_simple_triangle.c
	gcc -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2

uniform_var: common.c t5_uniform_var.c
	gcc -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

translation: common.c t6_translation.c
	gcc -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

rotation: common.c t7_rotation.c
	gcc -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

.PHONY: clean
clean:
	@rm -f $(OBJS)
