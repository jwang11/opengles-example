#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <wayland-client.h>
#include <wayland-server.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h> // Wayland EGL MUST be included before EGL headers

#include <EGL/egl.h>
#include <EGL/eglplatform.h>

#include <GLES2/gl2.h>

#define LOG(...) fprintf(stderr, __VA_ARGS__)
#define LOG_ERRNO(...)  fprintf(stderr, "Error : %s\n", strerror(errno)); fprintf(stderr, __VA_ARGS__)
struct wl_compositor *compositor = NULL;
struct wl_surface *surface;
struct wl_egl_window *egl_window;
struct wl_region *region;
struct wl_shell *shell;
struct wl_shell_surface *shell_surface;

struct _escontext
{
  /// Native System informations
  EGLNativeDisplayType native_display;
  EGLNativeWindowType native_window;
  uint16_t window_width, window_height;
  /// EGL display
  EGLDisplay  display;
  /// EGL context
  EGLContext  context;
  /// EGL surface
  EGLSurface  surface;

};

struct _escontext ESContext = {
  .native_display = NULL,
  .window_width = 0,
  .window_height = 0,
  .native_window  = 0,
  .display = NULL,
  .context = NULL,
  .surface = NULL
};

#define TRUE 1
#define FALSE 0

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

void CreateNativeWindow(char *title, int width, int height) {

  region = wl_compositor_create_region(compositor);

  wl_region_add(region, 0, 0, width, height);
  wl_surface_set_opaque_region(surface, region);

  struct wl_egl_window *egl_window = 
    wl_egl_window_create(surface, width, height);
    
  if (egl_window == EGL_NO_SURFACE) {
    LOG("No window !?\n");
    exit(1);
  }
  else LOG("Window created !\n");
  ESContext.window_width = width;
  ESContext.window_height = height;
  ESContext.native_window = egl_window;

}

EGLBoolean CreateEGLContext ()
{
   EGLint numConfigs;
   EGLint majorVersion;
   EGLint minorVersion;
   EGLContext context;
   EGLSurface surface;
   EGLConfig config;
   EGLint fbAttribs[] =
   {
       EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
       EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
       EGL_RED_SIZE,        8,
       EGL_GREEN_SIZE,      8,
       EGL_BLUE_SIZE,       8,
       EGL_NONE
   };
   EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
   EGLDisplay display = eglGetDisplay( ESContext.native_display );
   if ( display == EGL_NO_DISPLAY )
   {
      LOG("No EGL Display...\n");
      return EGL_FALSE;
   }

   // Initialize EGL
   if ( !eglInitialize(display, &majorVersion, &minorVersion) )
   {
      LOG("No Initialisation...\n");
      return EGL_FALSE;
   }

   // Get configs
   if ( (eglGetConfigs(display, NULL, 0, &numConfigs) != EGL_TRUE) || (numConfigs == 0))
   {
      LOG("No configuration...\n");
      return EGL_FALSE;
   }

   // Choose config
   if ( (eglChooseConfig(display, fbAttribs, &config, 1, &numConfigs) != EGL_TRUE) || (numConfigs != 1))
   {
      LOG("No configuration...\n");
      return EGL_FALSE;
   }

   // Create a surface
   surface = eglCreateWindowSurface(display, config, ESContext.native_window, NULL);
   if ( surface == EGL_NO_SURFACE )
   {
      LOG("No surface...\n");
      return EGL_FALSE;
   }

   // Create a GL context
   context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
   if ( context == EGL_NO_CONTEXT )
   {
      LOG("No context...\n");
      return EGL_FALSE;
   }

   // Make the context current
   if ( !eglMakeCurrent(display, surface, surface, context) )
   {
      LOG("Could not make the current window current !\n");
      return EGL_FALSE;
   }

   ESContext.display = display;
   ESContext.surface = surface;
   ESContext.context = context;
   return EGL_TRUE;
}

void shell_surface_ping
(void *data, struct wl_shell_surface *shell_surface, uint32_t serial) {
  wl_shell_surface_pong(shell_surface, serial);
}

void shell_surface_configure
(void *data, struct wl_shell_surface *shell_surface, uint32_t edges,
 int32_t width, int32_t height) {
  struct window *window = data;
  wl_egl_window_resize(ESContext.native_window, width, height, 0, 0);
}

void shell_surface_popup_done(void *data, struct wl_shell_surface *shell_surface) {
}

static struct wl_shell_surface_listener shell_surface_listener = {
  &shell_surface_ping,
  &shell_surface_configure,
  &shell_surface_popup_done
};

EGLBoolean CreateWindowWithEGLContext(char *title, int width, int height) {
  CreateNativeWindow(title, width, height);
  return CreateEGLContext();
}

void draw() {
  glClearColor(0.5, 0.3, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}

unsigned long last_click = 0;
void RefreshWindow() { eglSwapBuffers(ESContext.display, ESContext.surface); }

static void global_registry_handler
(void *data, struct wl_registry *registry, uint32_t id,
 const char *interface, uint32_t version) {
  LOG("Got a registry event for %s id %d\n", interface, id);
  if (strcmp(interface, "wl_compositor") == 0)
    compositor = 
      wl_registry_bind(registry, id, &wl_compositor_interface, 1);
  
  else if (strcmp(interface, "wl_shell") == 0)
    shell = wl_registry_bind(registry, id, &wl_shell_interface, 1);
}

static void global_registry_remover
(void *data, struct wl_registry *registry, uint32_t id) {
  LOG("Got a registry losing event for %d\n", id);
}

const struct wl_registry_listener listener = {
  global_registry_handler,
  global_registry_remover
};

static void
get_server_references() {

  struct wl_display * display = wl_display_connect(NULL);
  if (display == NULL) {
    LOG("Can't connect to wayland display !?\n");
    exit(1);
  }
  LOG("Got a display !");

  struct wl_registry *wl_registry =
    wl_display_get_registry(display);
  wl_registry_add_listener(wl_registry, &listener, NULL);
  
  // This call the attached listener global_registry_handler
  wl_display_dispatch(display);
  wl_display_roundtrip(display);

  // If at this point, global_registry_handler didn't set the 
  // compositor, nor the shell, bailout !
  if (compositor == NULL || shell == NULL) {
    LOG("No compositor !? No Shell !! There's NOTHING in here !\n");
    exit(1);
  }
  else {
    LOG("Okay, we got a compositor and a shell... That's something !\n");
    ESContext.native_display = display;
  }
}

void destroy_window() {
  eglDestroySurface(ESContext.display, ESContext.surface);
  wl_egl_window_destroy(ESContext.native_window);
  wl_shell_surface_destroy(shell_surface);
  wl_surface_destroy(surface);
  eglDestroyContext(ESContext.display, ESContext.context);
}

int main() {
  get_server_references();

  surface = wl_compositor_create_surface(compositor);
  if (surface == NULL) {
    LOG("No Compositor surface ! Yay....\n");
    exit(1);
  }
  else LOG("Got a compositor surface !\n");

  shell_surface = wl_shell_get_shell_surface(shell, surface);
  wl_shell_surface_set_toplevel(shell_surface);

  CreateWindowWithEGLContext("Nya", 1280, 720);

  while (1) {
    wl_display_dispatch_pending(ESContext.native_display);
    draw();
    RefreshWindow();
  }

  wl_display_disconnect(ESContext.native_display);
  LOG("Display disconnected !\n");
  
  exit(0);
}
