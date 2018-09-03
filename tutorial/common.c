#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include "util.h"


static EGLDisplay eglDisplay;
static EGLSurface eglSurface;

/*
 * Registry callbacks
 */
static void registry_global(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version)
{
    struct WaylandGlobals* globals = (struct WaylandGlobals *)data;
    if (strcmp(interface, "wl_compositor") == 0) {
        globals->compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else if (strcmp(interface, "wl_shell") == 0) {
        globals->shell = wl_registry_bind(registry, id, &wl_shell_interface, 1);
    }
}

static const struct wl_registry_listener registry_listener = { registry_global, NULL };

/*
 * Connect to the Wayland display and return the display and the surface
 * output wlDisplay
 * output wlSurface
 */
static void initWaylandDisplay(struct wl_display** wlDisplay, struct wl_surface** wlSurface)
{
    struct WaylandGlobals globals = {0};

    *wlDisplay = wl_display_connect(NULL);
    assert(*wlDisplay != NULL);

    struct wl_registry* registry = wl_display_get_registry(*wlDisplay);
    wl_registry_add_listener(registry, &registry_listener, (void *) &globals);

    wl_display_dispatch(*wlDisplay);
    wl_display_roundtrip(*wlDisplay);
    assert(globals.compositor);
    assert(globals.shell);

    *wlSurface = wl_compositor_create_surface(globals.compositor);
    assert(*wlSurface != NULL);

    struct wl_shell_surface* shellSurface = wl_shell_get_shell_surface(globals.shell, *wlSurface);
    wl_shell_surface_set_toplevel(shellSurface);
}

/*
 * Configure EGL and return necessary resources
 * input nativeDisplay
 * input nativeWindow
 * output eglDisplay
 * output eglSurface
 */
static void initEGLDisplay(EGLNativeDisplayType nativeDisplay, EGLNativeWindowType nativeWindow, EGLDisplay* eglDisp, EGLSurface* eglSurf)
{
    EGLint number_of_config;
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    *eglDisp = eglGetDisplay(nativeDisplay);
    assert(*eglDisp != EGL_NO_DISPLAY);

    EGLBoolean initialized = eglInitialize(*eglDisp, NULL, NULL);
    assert(initialized == EGL_TRUE);

    EGLConfig configs[1];

    eglChooseConfig(*eglDisp, config_attribs, configs, 1, &number_of_config);
    assert(number_of_config);

    EGLContext eglContext = eglCreateContext(*eglDisp, configs[0], EGL_NO_CONTEXT, context_attribs);

    *eglSurf = eglCreateWindowSurface(*eglDisp, configs[0], nativeWindow, NULL);
    assert(*eglSurf != EGL_NO_SURFACE);

    EGLBoolean makeCurrent = eglMakeCurrent(*eglDisp, *eglSurf, *eglSurf, eglContext);
    assert(makeCurrent == EGL_TRUE);
}

/*
 * Connect Wayland and make EGL
 * input width
 * input height
 * output wlDisplay
 * output eglDisplay
 * output eglSurface
 */
void initWindow(GLint width, GLint height, struct wl_display** wlDisplay)
{
    struct wl_surface* wlSurface;
    initWaylandDisplay(wlDisplay, &wlSurface);

    struct wl_egl_window* wlEglWindow = wl_egl_window_create(wlSurface, width, height);
    assert(wlEglWindow != NULL);

    initEGLDisplay((EGLNativeDisplayType) *wlDisplay, (EGLNativeWindowType) wlEglWindow, &eglDisplay, &eglSurface);
}

/*
static void delete_window (struct window *window) {
        eglDestroySurface (egl_display, window->egl_surface);
        wl_egl_window_destroy (window->egl_window);
        wl_shell_surface_destroy (window->shell_surface);
        wl_surface_destroy (window->surface);
        eglDestroyContext (egl_display, window->egl_context);
}
*/

/*
 * Return the loaded and compiled shader
 */
GLuint LoadShader(GLenum type, const char* shaderSrc)
{
    GLuint shader = glCreateShader(type);
    assert(shader);

    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
	if (type == GL_VERTEX_SHADER)
            fprintf(stderr, "Error compiling vetex shader: '%s'\n", InfoLog);
	if (type == GL_FRAGMENT_SHADER)
            fprintf(stderr, "Error compiling fragment shader: '%s'\n", InfoLog);
        exit(1);
    }

    return shader;
}

void RefreshWindow(void)
{
    eglSwapBuffers(eglDisplay, eglSurface);
}
