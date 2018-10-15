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

GLuint VBO;
Vector3f vVertices[] = { 
    Vector3f(0.0f, 1.0f, 0.0f),
    Vector3f(-1.0f, -1.0f, 0.0f),
    Vector3f(1.0f, -1.0f, 0.0f)
};


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
    glUseProgram(programObject);

    return programObject;
}

static void CreateVertexBuffer()
{
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);
}

/*
 * Draw a triangle
 */
void draw(GLint width, GLint height)
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vVertices)/sizeof(Vector3f));
    glDisableVertexAttribArray(0);
}

int main(int argc, char** argv)
{
    int width = 320;
    int height = 240;

    struct wl_display* wlDisplay;

    initWindow(width, height, &wlDisplay);

    CreateVertexBuffer();
    GLuint programObject = initProgramObject();

    glViewport(0, 0, width, height);
    while (1) {
        wl_display_dispatch_pending(wlDisplay);
        draw(width, height);
        RefreshWindow();
    }

    glDeleteProgram(programObject);

    wl_display_disconnect(wlDisplay);

    return 0;
}
