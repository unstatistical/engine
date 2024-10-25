#ifndef ENGINE_WIN32_PLATFORM_H_
#define ENGINE_WIN32_PLATFORM_H_

#include "EGL/egl.h"
#include "windows.h"

typedef struct {
  HDC display;
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
  HWND window;
  struct {
    EGLSurface surface;
  } egl;
} WindowContext;

int createDisplayContext(DisplayContext *);
int createWindowContext(DisplayContext *, WindowContext *);
void destroyWindowContext(WindowContext *);

#endif