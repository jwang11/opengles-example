/*
 * A simple Wayland EGL program to show a triangle
 *
 */

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <assert.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include "util.h"

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
