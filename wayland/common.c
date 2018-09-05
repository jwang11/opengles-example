#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-cursor.h>

#include "util.h"


static EGLDisplay eglDisplay;
static EGLSurface eglSurface;

uint32_t press_key = 0;
uint32_t pointer_x = 0;
uint32_t pointer_y = 0;

static void
keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
                       uint32_t format, int fd, uint32_t size)
{
}

static void
keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
                      uint32_t serial, struct wl_surface *surface,
                      struct wl_array *keys)
{
}

static void
keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
                      uint32_t serial, struct wl_surface *surface)
{
}

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
                    uint32_t serial, uint32_t time, uint32_t key,
                    uint32_t state)
{
    if (state == 1)
        press_key = key;
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
                          uint32_t serial, uint32_t mods_depressed,
                          uint32_t mods_latched, uint32_t mods_locked,
                          uint32_t group)
{
}

static const struct wl_keyboard_listener keyboard_listener = {
    keyboard_handle_keymap,
    keyboard_handle_enter,
    keyboard_handle_leave,
    keyboard_handle_key,
    keyboard_handle_modifiers,
};

static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface,
                     wl_fixed_t sx, wl_fixed_t sy)
{
}

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface)
{
}

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
                      uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    pointer_x = sx;
    pointer_y = sy;
}

static void
pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
                      uint32_t serial, uint32_t time, uint32_t button,
                      uint32_t state)
{
}

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                    uint32_t time, uint32_t axis, wl_fixed_t value)
{
}

static const struct wl_pointer_listener pointer_listener = {
        pointer_handle_enter,
        pointer_handle_leave,
        pointer_handle_motion,
        pointer_handle_button,
        pointer_handle_axis,
};

static void
seat_handle_capabilities(void *data, struct wl_seat *seat,
                         uint32_t caps)
{
    struct WaylandGlobals* d = (struct WaylandGlobals *)data;
    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !d->pointer) {
        d->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(d->pointer, &pointer_listener, d);
    } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && d->pointer) {
        wl_pointer_destroy(d->pointer);
        d->pointer = NULL;
    }
    
    if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !d->keyboard) {
        d->keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(d->keyboard, &keyboard_listener, d);
    } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && d->keyboard) {
        wl_keyboard_destroy(d->keyboard);
        d->keyboard = NULL;
    }
}

static const struct wl_seat_listener seat_listener = {
        seat_handle_capabilities,
};

/*
 * Registry callbacks
 */
static void registry_global(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version)
{
    struct WaylandGlobals* globals = (struct WaylandGlobals *)data;
    if (strcmp(interface, "wl_compositor") == 0) {
        globals->compositor = (struct wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else if (strcmp(interface, "wl_shell") == 0) {
        globals->shell = (struct wl_shell *)wl_registry_bind(registry, id, &wl_shell_interface, 1);
    } else if (strcmp(interface, "wl_seat") == 0) {
        globals->seat = (struct wl_seat *)wl_registry_bind(registry, id,
                                   &wl_seat_interface, 1);
        wl_seat_add_listener(globals->seat, &seat_listener, globals);    
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
