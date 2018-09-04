OBJS := simple_triangle \
	uniform_var translation \
	rotation \
	scaling \
	interpolation \
	indexed_draw \
	contatenate_transf \
	perspective_projection \
	camera_space

all: $(OBJS)
	@echo $(OBJS)

simple_triangle: common.c t4_simple_triangle.c
	g++ -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2

uniform_var: common.c t5_uniform_var.c
	g++ -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

translation: common.c t6_translation.c
	g++ -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

rotation: common.c t7_rotation.c
	g++ -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

scaling: common.c t8_scaling.c
	g++ -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

interpolation: common.c t9_interpolation.c
	g++ -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

indexed_draw: common.c t10_indexed_draw.c
	g++ -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

contatenate_transf: common.c math_3d.c pipeline.c t11_contatenate_transf.c 
	g++ -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

perspective_projection: common.c math_3d.c pipeline.c t12_perspective_projection.c 
	g++ -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

camera_space: common.c math_3d.c pipeline.c camera.c t13_camera_space.c 
	g++ -o $@ $^ -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lm

.PHONY: clean
clean:
	@rm -f $(OBJS)
