/*
 * A simple Wayland EGL program to show uniform variable
 *
 */

#include <string.h>
#include <assert.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "util.h"
#include <math.h>

GLuint gScaleLocation;

/*
 * Initialize the shaders and return the program object
 */
GLuint initProgramObject()
{
    char vShaderStr[] = "#version 300 es                          \n"
                        "layout(location = 0) in vec4 Position;  \n"
			"uniform float gScale;  \n"
                        "void main()                              \n"
                        "{                                        \n"
                        "   gl_Position = vec4(gScale * Position.x, gScale * Position.y, Position.z, 1.0);              \n"
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

/*
 * Draw a triangle which change size dynamically
 */
void draw(GLint width, GLint height)
{
    static float Scale = 0.0f;
    Scale += 0.01f;
    glUniform1f(gScaleLocation, sinf(Scale));
    GLfloat vVertices[] = { 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f };

    glViewport(0, 0, width, height);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(int argc, char** argv)
{
    int width = 320;
    int height = 240;

    struct wl_display* wlDisplay;

    initWindow(width, height, &wlDisplay);

    GLuint programObject = initProgramObject();
    gScaleLocation = glGetUniformLocation(programObject, "gScale");
    assert(gScaleLocation != 0xFFFFFFFF);

    while (1) {
        wl_display_dispatch_pending(wlDisplay);
        draw(width, height);
	RefreshWindow();
    }

    glDeleteProgram(programObject);

    wl_display_disconnect(wlDisplay);

    return 0;
}
