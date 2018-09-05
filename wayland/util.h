#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "math_3d.h"

#ifdef __cplusplus
extern "C" {
#endif
struct WaylandGlobals {
    struct wl_compositor* compositor;
    struct wl_shell* shell;
    struct wl_seat *seat;
    struct wl_keyboard *keyboard;
    struct wl_pointer *pointer;
};


void initWindow(GLint width, GLint height, struct wl_display** wlDisplay);
GLuint LoadShader(GLenum type, const char* shaderSrc);
void RefreshWindow(void);

#ifdef __cplusplus
}
#endif

#endif // UTIL_H
