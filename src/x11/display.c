#include <stdio.h>
#include <stdlib.h>

#include "x11/platform.h"

int createDisplayContext(DisplayContext *display) {
  display->display = XOpenDisplay(NULL);
  if (display->display == None) {
    printf("Failed to open display.\n");
    return 0;
  }
  display->root = DefaultRootWindow(display->display);
  if (display->root == None) {
    printf("Failed to get root window.\n");
    return 0;
  }
  display->hints.input = True;
  display->hints.flags = InputHint;
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
  display->egl.display = eglGetDisplay((EGLNativeDisplayType) display->display);
  if (display->egl.display == EGL_NO_DISPLAY) {
    printf("Failed to get display.\n");
    return 0;
  }
  if (eglInitialize(display->egl.display, &maj_version, &min_version) == EGL_FALSE) {
    printf("Failed to initialize display.\n");
    return 0;
  }
  if ((eglGetConfigs(display->egl.display, NULL, 0, &num_configs) != EGL_TRUE) || (num_configs == 0)) {
    printf("Failed to get configuration.\n");
    return 0;
  }
  if ((eglChooseConfig(display->egl.display, fb_attribs, &display->egl.config, 1, &num_configs) != EGL_TRUE) || (num_configs == 0)) {
    printf("Failed to choose configuration.\n");
    return 0;
  }
  display->egl.context = eglCreateContext(display->egl.display, display->egl.config, EGL_NO_CONTEXT, context_attribs);
  if (display->egl.context == EGL_NO_CONTEXT) {
    printf("Failed to get context from configuration.\n");
    return 0;
  }
  return 1;
}