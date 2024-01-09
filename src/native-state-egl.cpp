#include "native-state-egl.h"

#include <vector>

#include "libmatrix/log.h"
#include "options.h"
#include "util.h"

std::string const gpu_id_opt{"gpu"};

EGLint get_gpu_id_option() {
    EGLint gpu_id = 0;

    for (auto const& opt : Options::winsys_options) {
        if (opt.name == gpu_id_opt)
            gpu_id = Util::fromString<EGLint>(opt.value);
    }

    return gpu_id;
}

NativeStateEGL::NativeStateEGL() : eglDevice(nullptr), gpuId(-1) {
    Options::winsys_options_help =
        "  gpu=id  GPU the benchmark will run on \n";
}

bool NativeStateEGL::init_display() {
    gpuId = gpuId > 0 ? gpuId : get_gpu_id_option();
#if defined(WIN32)
    if (!egl_lib_.open("libEGL.dll")) {
#else
    if (!egl_lib_.open_from_alternatives({"libEGL.so", "libEGL.so.1"})) {
#endif
        Log::error("Error loading EGL library\n");
        return false;
    }
    PFNEGLGETPROCADDRESSPROC egl_get_proc_address =
        reinterpret_cast<PFNEGLGETPROCADDRESSPROC>(egl_lib_.load("eglGetProcAddress"));
    if (!egl_get_proc_address) return false;

    PFNEGLQUERYDEVICESEXTPROC egl_query_devices = reinterpret_cast<PFNEGLQUERYDEVICESEXTPROC>(egl_get_proc_address("eglQueryDevicesEXT"));
    if (!egl_query_devices) {
        Log::error("Error: Failed to get function pointers for EGL extensions! \n");
        return false;
    }
    EGLint numDevices;
    egl_query_devices(0, NULL, &numDevices);
    if (gpuId >= numDevices) {
        Log::error("Error: Failed to find the device!\n");
        return false;
    }

    std::vector<EGLDeviceEXT> devices(numDevices);
    egl_query_devices(numDevices, &devices[0], &numDevices);

    eglDevice = devices[gpuId];
    return true;
}

void* NativeStateEGL::display() {
    return (void*)eglDevice;
}

bool NativeStateEGL::create_window(WindowProperties const& properties) {
    pbufferAttribs[0] = EGL_WIDTH;
    pbufferAttribs[1] = properties.width;
    pbufferAttribs[2] = EGL_HEIGHT;
    pbufferAttribs[3] = properties.height;
    pbufferAttribs[4] = EGL_NONE;
    properties_ = properties;
    return true;
}

void* NativeStateEGL::window(WindowProperties& properties) {
    properties = properties_;
    return (void*)pbufferAttribs;
}