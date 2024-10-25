#include <stdio.h>

#include "windows.h"

#include "win32/platform.h"

int createWindowContext(DisplayContext *display, WindowContext *window) {
  window->display = display;
  window->window = createWindow(window->w, window->h);
  window->egl.surface = eglCreateWindowSurface(display->egl.display, display->egl.config, (EGLNativeWindowType) window->window, NULL);
  if (window->egl.surface == EGL_NO_SURFACE) {
    printf("Failed to get surface from display window.\n");
    return 0;
  }
  if (eglMakeCurrent(display->egl.display, window->egl.surface, window->egl.surface, display->egl.context) == EGL_FALSE) {
    printf("Failed to make window current.\n");
    return 0;
  }
  return 1;
}

void destroyWindowContext(WindowContext *window) {
  eglDestroySurface(window->display->egl.display, window->egl.surface);
}