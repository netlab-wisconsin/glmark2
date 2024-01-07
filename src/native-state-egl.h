
#ifndef GLMARK2_NATIVE_STATE_EGL_H_
#define GLMARK2_NATIVE_STATE_EGL_H_

#include "glad/egl.h"

#include "native-state.h"
#include "shared-library.h"

class NativeStateEGL : public NativeState {
   public:
    NativeStateEGL();
    ~NativeStateEGL() {}

    bool init_display();
    void* display();
    bool create_window(WindowProperties const& properties);
    void* window(WindowProperties& properties);
    void visible(bool v) {}
    bool should_quit() { return false; }
    void flip() {}

   private:
    EGLDeviceEXT eglDevice;
    EGLint gpuId;
    EGLint pbufferAttribs[5];
    SharedLibrary egl_lib_;
    WindowProperties properties_;
};
#endif