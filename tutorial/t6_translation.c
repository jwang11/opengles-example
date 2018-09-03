/*
 * A simple Wayland EGL program to show uniform variable
 *
 */

#include <string.h>
#include <math.h>
#include <assert.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "util.h"

GLuint gWorldLocation;
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
                        "layout(location = 0) in vec3 Position;   \n"
			"uniform mat4 gWorld;                     \n"
                        "void main()                              \n"
                        "{                                        \n"
                        "   gl_Position = gWorld * vec4(Position, 1.0);   \n"
                        "}                                                \n";

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
 * Draw a triangle which change size dynamically
 */
void draw(GLint width, GLint height)
{
    static float Scale = 0.0f;

    Scale += 0.01f;
    struct Matrix4f World;

    World.m[0][0] = 1.0f; World.m[0][1] = 0.0f; World.m[0][2] = 0.0f; World.m[0][3] = sinf(Scale);
    World.m[1][0] = 0.0f; World.m[1][1] = 1.0f; World.m[1][2] = 0.0f; World.m[1][3] = 0.0f;
    World.m[2][0] = 0.0f; World.m[2][1] = 0.0f; World.m[2][2] = 1.0f; World.m[2][3] = 0.0f;
    World.m[3][0] = 0.0f; World.m[3][1] = 0.0f; World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;
    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);

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
    gWorldLocation = glGetUniformLocation(programObject, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);

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
