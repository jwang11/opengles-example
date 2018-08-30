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

void initWindow(GLint width, GLint height, struct wl_display** wlDisplay, EGLDisplay* eglDisplay, EGLSurface* eglSurface);
GLuint LoadShader(GLenum type, const char* shaderSrc);

#ifdef __cplusplus
}
#endif

#endif // UTIL_H
