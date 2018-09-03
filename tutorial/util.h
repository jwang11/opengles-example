#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifdef __cplusplus
extern "C" {
#endif
struct WaylandGlobals {
    struct wl_compositor* compositor;
    struct wl_shell* shell;
};

struct Matrix4f {
    float m[4][4];
};

struct Vector3f
{
	float x;
	float y;
	float z;

	Vector3f(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

void initWindow(GLint width, GLint height, struct wl_display** wlDisplay);
GLuint LoadShader(GLenum type, const char* shaderSrc);
void RefreshWindow(void);

#ifdef __cplusplus
}
#endif

#endif // UTIL_H
