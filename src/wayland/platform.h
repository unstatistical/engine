#ifndef ENGINE_WAYLAND_PLATFORM_H_
#define ENGINE_WAYLAND_PLATFORM_H_

#include "wayland-client.h"
#include "wayland-egl.h"
#include "EGL/egl.h"

typedef struct {
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_compositor *compositor;
  struct {
    struct xdg_wm_base *wm_base;
  } xdg;
  struct {
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
  } egl;
} DisplayContext;

typedef struct {
  DisplayContext *display;
  int w, h, x, y;
  const char *title;
  int alive;
  struct wl_surface *surface;
  struct wl_region *region;
  struct {
    struct xdg_surface *surface;
    struct xdg_toplevel *toplevel;
  } xdg;
  struct {
    struct wl_egl_window *window;
    EGLSurface surface;
  } egl;
} WindowContext;

int createDisplayContext(DisplayContext *);
int createWindowContext(DisplayContext *, WindowContext *);
void destroyWindowContext(WindowContext *);

#endif