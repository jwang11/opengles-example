OBJS := simple_triangle uniform_var translation rotation scaling interpolation indexed_draw
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

scaling: common.c t8_scaling.c
	gcc -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

interpolation: common.c t9_interpolation.c
	gcc -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

indexed_draw: common.c t10_indexed_draw.c
	gcc -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

.PHONY: clean
clean:
	@rm -f $(OBJS)
