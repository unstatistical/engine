#include <string.h>
#include <stdio.h>

#include "EGL/egl.h"
#include "wayland-client.h"
#include "wayland-egl.h"

#include "xdg-shell-client-protocol.h"

#include "wayland/platform.h"

static void wmBaseHandlePing(void *data, struct xdg_wm_base *wm_base, uint32_t serial) {
  xdg_wm_base_pong(wm_base, serial);
}

static struct xdg_wm_base_listener wm_base_listener = {
  .ping = wmBaseHandlePing
};

static void registryHandleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
  DisplayContext *display = (DisplayContext *) data;
  if (strcmp(interface, "wl_compositor") == 0) {
    display->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
  } else if (strcmp(interface, "xdg_wm_base") == 0) {
    display->xdg.wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener(display->xdg.wm_base, &wm_base_listener, NULL);
  }
}

static struct wl_registry_listener registry_listener = {
  .global = registryHandleGlobal
};

int createDisplayContext(DisplayContext *display) {
  display->display = wl_display_connect(NULL);
  if (display->display == NULL) {
    printf("Failed to connect display.\n");
    return 0;
  }
  display->registry = wl_display_get_registry(display->display);
  if (display->registry == NULL) {
    printf("Failed to get registry from display.\n");
    return 0;
  }
  wl_registry_add_listener(display->registry, &registry_listener, display);
  wl_display_dispatch(display->display);
  wl_display_roundtrip(display->display);
  if (display->compositor == NULL || display->xdg.wm_base == NULL) {
    if (display->compositor == NULL) {
      printf("Failed to get compositor from display registry.\n");
    }
    if (display->xdg.wm_base == NULL) {
      printf("Failed to get wm_base from display registry.\n");
    }
    return 0;
  }
  EGLint num_configs;
  EGLint min_version;
  EGLint maj_version;
  EGLint fb_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_NONE
  };
  EGLint context_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE,
    EGL_NONE
  };
  display->egl.display = eglGetDisplay(display->display);
  if (display->egl.display == EGL_NO_DISPLAY) {
    printf("Failed to get display from display.\n");
    return 0;
  }
  if (eglInitialize(display->egl.display, &maj_version, &min_version) == EGL_FALSE) {
    printf("Failed to initialize display.\n");
    return 1;
  }
  if ((eglGetConfigs(display->egl.display, NULL, 0, &num_configs) != EGL_TRUE) || (num_configs == 0)) {
    printf("Failed to get configuration.\n");
    return 0;
  }
  if ((eglChooseConfig(display->egl.display, fb_attribs, &display->egl.config, 1, &num_configs) != EGL_TRUE) || (num_configs != 1)) {
    printf("Failed to choose configuration.\n");
    return 0;
  }
  display->egl.context = eglCreateContext(display->egl.display, display->egl.config, EGL_NO_CONTEXT, context_attribs);
  if (display->egl.context == EGL_NO_CONTEXT) {
    printf("Failed to create context from display configuration.\n");
    return 0;
  }
  return 1;
}