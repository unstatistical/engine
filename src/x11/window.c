#include "X11/Xlib.h"
#include "X11/Xatom.h"
#include "X11/Xutil.h"

#include "x11/platform.h"

int createWindowContext(DisplayContext *display, WindowContext *window) {
  window->display = display;
  window->attributes.event_mask = ExposureMask | ButtonPressMask | KeyPressMask;
  window->window = XCreateWindow(display->display, display->root, window->w, window->y, window->w, window->h, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &window->attributes);
  int data = 1;
  XChangeProperty(display->display, window->window, XInternAtom(display->display, "_HILDON_NON_COMPOSITED_WINDOW", False), XA_INTEGER, 32, PropModeReplace, (unsigned char *) &data, 1);
  XSetWMHints(display->display, window->window, &display->hints);
  XMapWindow(display->display, window->window);
  XStoreName(display->display, window->window, window->title);
  window->egl.surface = eglCreateWindowSurface(display->egl.display, display->egl.config, window->window, NULL);
  eglMakeCurrent(display->egl.display, window->egl.surface, window->egl.surface, display->egl.context);
  window->alive = 1;
}

void destroyWindowContext(WindowContext *window) {
  XFree(window->window);
}