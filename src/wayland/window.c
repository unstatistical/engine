#include <stdio.h>

#include "EGL/egl.h"
#include "GL/gl.h"
#include "wayland-client.h"
#include "wayland-egl.h"

#include "xdg-shell-client-protocol.h"

#include "wayland/platform.h"

static void toplevelHandleConfigure(void *data, struct xdg_toplevel *toplevel, int32_t w, int32_t h, struct wl_array *states) {
  WindowContext *window = (WindowContext *) data;
  if (w == 0 && h == 0) {
    return;
  }
  if (window->w != w && window->h != h) {
    window->w = w;
    window->h = h;
    wl_egl_window_resize(window->egl.window, w, h, 0, 0);
    wl_surface_commit(window->surface);
  }
}

static void toplevelHandleClose(void *data, struct xdg_toplevel *toplevel) {
  WindowContext *window = (WindowContext *) data;
  window->alive = 0;
}

static struct xdg_toplevel_listener toplevel_listener = {
  .configure = toplevelHandleConfigure,
  .close = toplevelHandleClose
};

static void surfaceHandleConfigure(void *data, struct xdg_surface *surface, uint32_t serial) {
  xdg_surface_ack_configure(surface, serial);
}

static struct xdg_surface_listener surface_listener = {
  .configure = surfaceHandleConfigure
};

int createWindowContext(DisplayContext *display, WindowContext *window) {
  window->display = display;
  window->surface = wl_compositor_create_surface(display->compositor);
  if (window->surface == NULL) {
    printf("Failed to create surface from compositor.\n");
    return 0;
  }
  window->xdg.surface = xdg_wm_base_get_xdg_surface(display->xdg.wm_base, window->surface);
  if (window->xdg.surface == NULL) {
    printf("Failed to create surface from surface.\n");
    return 0;
  }
  xdg_surface_add_listener(window->xdg.surface, &surface_listener, NULL);
  window->xdg.toplevel = xdg_surface_get_toplevel(window->xdg.surface);
  if (window->xdg.toplevel == NULL) {
    printf("Failed to create toplevel from surface.\n");
    return 1;
  }
  xdg_toplevel_add_listener(window->xdg.toplevel, &toplevel_listener, window);
  xdg_toplevel_set_title(window->xdg.toplevel, window->title);
  window->egl.window = wl_egl_window_create(window->surface, window->w, window->h);
  if (window->egl.window == EGL_NO_SURFACE) {
    printf("Failed to create window from surface.\n");
    return 0;
  }
  window->egl.surface = eglCreateWindowSurface(display->egl.display, display->egl.config, (EGLNativeWindowType) window->egl.window, NULL);
  if (window->egl.surface == EGL_NO_SURFACE) {
    printf("Failed to get surface from display window.\n");
    return 0;
  }
  window->region = wl_compositor_create_region(display->compositor);
  if (window->region == NULL) {
    printf("Failed to create region.\n");
    return 0;
  }
  wl_region_add(window->region, window->x, window->y, window->w, window->h);
  wl_surface_set_opaque_region(window->surface, window->region);
  if (eglMakeCurrent(display->egl.display, window->egl.surface, window->egl.surface, display->egl.context) == EGL_FALSE) {
    printf("Failed to make window current.\n");
    return 0;
  }
  window->alive = 1;
  return 1;
}

void destroyWindowContext(DisplayContext *display, WindowContext *window) {
  eglDestroySurface(display->egl.display, window->egl.surface);
  wl_egl_window_destroy(window->egl.window);
  xdg_toplevel_destroy(window->xdg.toplevel);
  xdg_surface_destroy(window->xdg.surface);
  wl_region_destroy(window->region);
  wl_surface_destroy(window->surface);
  eglDestroyContext(display->egl.display, window->egl.surface);
}