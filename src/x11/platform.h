#ifndef ENGINE_X11_PLATFORM_H_
#define ENGINE_X11_PLATFORM_H_

#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "EGL/egl.h"

typedef struct {
  Display *display;
  Window root;
  XWMHints hints;
  struct {
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
  } egl;
} DisplayContext;

typedef struct {
  DisplayContext *display;
  int x, y, w, h;
  const char *title;
  int alive;
  Window window;
  XSetWindowAttributes attributes;
  struct {
    EGLSurface surface;
  } egl;
} WindowContext;

int createDisplayContext(DisplayContext *);
int createWindowContext(DisplayContext *, WindowContext *);
void destroyWindowContext(WindowContext *);

#endif