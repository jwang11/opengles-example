/*
 * A simple Wayland EGL program to show a triangle
 *
 * cc -o triangle_simple triangle_simple.c -lwayland-client -lwayland-egl -lEGL -lGLESv2
 */

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <assert.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-egl.h>

struct WaylandGlobals {
    struct wl_compositor* compositor;
    struct wl_shell* shell;
};
    
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
static void initEGLDisplay(EGLNativeDisplayType nativeDisplay, EGLNativeWindowType nativeWindow, EGLDisplay* eglDisplay, EGLSurface* eglSurface)
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

    *eglDisplay = eglGetDisplay(nativeDisplay);
    assert(*eglDisplay != EGL_NO_DISPLAY);

    EGLBoolean initialized = eglInitialize(*eglDisplay, NULL, NULL);
    assert(initialized == EGL_TRUE);

    EGLConfig configs[1];

    eglChooseConfig(*eglDisplay, config_attribs, configs, 1, &number_of_config);
    assert(number_of_config);

    EGLContext eglContext = eglCreateContext(*eglDisplay, configs[0], EGL_NO_CONTEXT, context_attribs);

    *eglSurface = eglCreateWindowSurface(*eglDisplay, configs[0], nativeWindow, NULL);
    assert(*eglSurface != EGL_NO_SURFACE);

    EGLBoolean makeCurrent = eglMakeCurrent(*eglDisplay, *eglSurface, *eglSurface, eglContext);
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
static void initWindow(GLint width, GLint height, struct wl_display** wlDisplay, EGLDisplay* eglDisplay, EGLSurface* eglSurface)
{
    struct wl_surface* wlSurface;
    initWaylandDisplay(wlDisplay, &wlSurface);

    struct wl_egl_window* wlEglWindow = wl_egl_window_create(wlSurface, width, height);
    assert(wlEglWindow != NULL);

    initEGLDisplay((EGLNativeDisplayType) *wlDisplay, (EGLNativeWindowType) wlEglWindow, eglDisplay, eglSurface);
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
    assert(compiled);

    return shader;
}

/*
 * Initialize the shaders and return the program object
 */
GLuint initProgramObject()
{
    char vShaderStr[] = "#version 300 es                          \n"
                        "layout(location = 0) in vec4 vPosition;  \n"
                        "void main()                              \n"
                        "{                                        \n"
                        "   gl_Position = vPosition;              \n"
                        "}                                        \n";

    char fShaderStr[] = "#version 300 es                              \n"
                        "precision mediump float;                     \n"
                        "out vec4 fragColor;                          \n"
                        "void main()                                  \n"
                        "{                                            \n"
                        "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
                        "}                                            \n";

    GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    GLuint programObject = glCreateProgram();
    assert(programObject);

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

    GLint linked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    assert(linked);

    return programObject;
}

/*
 * Draw a triangle
 */
void draw(GLuint programObject, GLint width, GLint height)
{
    GLfloat vVertices[] = { 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f };

    glViewport(0, 0, width, height);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(int argc, char** argv)
{
    int width = 320;
    int height = 240;

    struct wl_display* wlDisplay;
    EGLDisplay eglDisplay;
    EGLSurface eglSurface;

    initWindow(width, height, &wlDisplay, &eglDisplay, &eglSurface);

    GLuint programObject = initProgramObject();
    assert(programObject);

    draw(programObject, width, height);
    eglSwapBuffers(eglDisplay, eglSurface);

    while (wl_display_dispatch(wlDisplay) != -1) {
    }

    glDeleteProgram(programObject);

    wl_display_disconnect(wlDisplay);

    return 0;
}
