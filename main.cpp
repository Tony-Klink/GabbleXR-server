#include <iostream>
#include <vector>
#include <string.h>
#include <ctime>
#include <iomanip>
#include <lo/lo.h>

#define XR_USE_PLATFORM_LINUX 1
#define XR_USE_TIMESPEC 1
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

// Function pointers for XR_ANDROID_face_tracking
PFN_xrCreateFaceTrackerANDROID xrCreateFaceTrackerANDROID_ptr = nullptr;
PFN_xrDestroyFaceTrackerANDROID xrDestroyFaceTrackerANDROID_ptr = nullptr;
PFN_xrGetFaceStateANDROID xrGetFaceStateANDROID_ptr = nullptr;

// Function pointer for XR_KHR_convert_timespec_time
PFN_xrConvertTimespecTimeToTimeKHR xrConvertTimespecTimeToTimeKHR_ptr = nullptr;

#define CHK_XR(res) if (res != XR_SUCCESS) { std::cerr << "OpenXR Error: " << res << " at " << __LINE__ << std::endl; return 1; }

void send_v2_parameters(lo_address addr, float* p) {
    // Eye Gaze
    lo_send(addr, "/v2/EyeLeftX", "f", p[18] - p[16]); // Right - Left
    lo_send(addr, "/v2/EyeLeftY", "f", p[20] - p[14]); // Up - Down
    lo_send(addr, "/v2/EyeRightX", "f", p[19] - p[17]);
    lo_send(addr, "/v2/EyeRightY", "f", p[21] - p[15]);

    // Eye Lids (VRCFT: 0 closed, 0.75 open, 1.0 widen)
    // Android: EyesClosed is 1.0 at closed. UpperLidRaiser is 1.0 at widen.
    float lidL = (1.0f - p[12]) * 0.75f + p[59] * 0.25f;
    float lidR = (1.0f - p[13]) * 0.75f + p[60] * 0.25f;
    lo_send(addr, "/v2/EyeLidLeft", "f", lidL);
    lo_send(addr, "/v2/EyeLidRight", "f", lidR);

    // Eye Squint
    lo_send(addr, "/v2/EyeSquintLeft", "f", p[28]);
    lo_send(addr, "/v2/EyeSquintRight", "f", p[29]);

    // Brow
    lo_send(addr, "/v2/BrowInnerUpLeft", "f", p[22]);
    lo_send(addr, "/v2/BrowInnerUpRight", "f", p[23]);
    lo_send(addr, "/v2/BrowOuterUpLeft", "f", p[57]);
    lo_send(addr, "/v2/BrowOuterUpRight", "f", p[58]);
    lo_send(addr, "/v2/BrowLowererLeft", "f", p[0]);
    lo_send(addr, "/v2/BrowLowererRight", "f", p[1]);

    // Jaw
    lo_send(addr, "/v2/JawOpen", "f", p[24]);
    lo_send(addr, "/v2/JawX", "f", p[26] - p[25]); // Right - Left
    lo_send(addr, "/v2/JawZ", "f", p[27]);

    // Mouth
    lo_send(addr, "/v2/MouthUpperUpLeft", "f", p[61]);
    lo_send(addr, "/v2/MouthUpperUpRight", "f", p[62]);
    lo_send(addr, "/v2/MouthLowerDownLeft", "f", p[51]);
    lo_send(addr, "/v2/MouthLowerDownRight", "f", p[52]);
    lo_send(addr, "/v2/MouthCornerPullLeft", "f", p[32]);
    lo_send(addr, "/v2/MouthCornerPullRight", "f", p[33]);
    lo_send(addr, "/v2/MouthFrownLeft", "f", p[30]);
    lo_send(addr, "/v2/MouthFrownRight", "f", p[31]);
    lo_send(addr, "/v2/MouthStretchLeft", "f", p[42]);
    lo_send(addr, "/v2/MouthStretchRight", "f", p[43]);
    lo_send(addr, "/v2/MouthDimpleLeft", "f", p[10]);
    lo_send(addr, "/v2/MouthDimpleRight", "f", p[11]);
    lo_send(addr, "/v2/MouthPressLeft", "f", p[38]);
    lo_send(addr, "/v2/MouthPressRight", "f", p[39]);
    lo_send(addr, "/v2/MouthTightenerLeft", "f", p[48]);
    lo_send(addr, "/v2/MouthTightenerRight", "f", p[49]);
    lo_send(addr, "/v2/MouthClosed", "f", p[50]);

    // Lip
    lo_send(addr, "/v2/LipPuckerUpperLeft", "f", p[40]);
    lo_send(addr, "/v2/LipPuckerUpperRight", "f", p[41]);
    lo_send(addr, "/v2/LipPuckerLowerLeft", "f", p[40]); // Android doesn't distinguish upper/lower pucker?
    lo_send(addr, "/v2/LipPuckerLowerRight", "f", p[41]);
    
    lo_send(addr, "/v2/LipFunnelUpperLeft", "f", p[35]);
    lo_send(addr, "/v2/LipFunnelUpperRight", "f", p[37]);
    lo_send(addr, "/v2/LipFunnelLowerLeft", "f", p[34]);
    lo_send(addr, "/v2/LipFunnelLowerRight", "f", p[36]);

    lo_send(addr, "/v2/LipSuckUpperLeft", "f", p[45]);
    lo_send(addr, "/v2/LipSuckUpperRight", "f", p[47]);
    lo_send(addr, "/v2/LipSuckLowerLeft", "f", p[44]);
    lo_send(addr, "/v2/LipSuckLowerRight", "f", p[46]);

    // Cheek
    // CheekPuffSuck: Puff (0 to 1) / Suck (0 to -1)
    lo_send(addr, "/v2/CheekPuffSuckLeft", "f", p[2] - p[6]);
    lo_send(addr, "/v2/CheekPuffSuckRight", "f", p[3] - p[7]);
    lo_send(addr, "/v2/CheekSquintLeft", "f", p[4]);
    lo_send(addr, "/v2/CheekSquintRight", "f", p[5]);

    // Tongue
    lo_send(addr, "/v2/TongueOut", "f", p[63]);
    lo_send(addr, "/v2/TongueX", "f", p[65] - p[64]); // Right - Left
    lo_send(addr, "/v2/TongueY", "f", p[66] - p[67]); // Up - Down

    // Tracking status
    lo_send(addr, "/v2/EyeTrackingActive", "i", 1);
    lo_send(addr, "/v2/LipTrackingActive", "i", 1);
    lo_send(addr, "/v2/ExpressionTrackingActive", "i", 1);
}

int main() {
    // Initialize OSC
    lo_address resoniteAddr = lo_address_new("127.0.0.1", "8888");
    if (!resoniteAddr) {
        std::cerr << "Failed to initialize OSC address" << std::endl;
        return 1;
    }

    // 1. Get extensions
    uint32_t extCount = 0;
    xrEnumerateInstanceExtensionProperties(nullptr, 0, &extCount, nullptr);
    std::vector<XrExtensionProperties> extensions(extCount, {XR_TYPE_EXTENSION_PROPERTIES});
    xrEnumerateInstanceExtensionProperties(nullptr, extCount, &extCount, extensions.data());

    std::vector<const char*> enabledExtensions;
    bool faceTrackingSupported = false;
    bool timespecSupported = false;

    for (const auto& ext : extensions) {
        if (strcmp(ext.extensionName, "XR_ANDROID_face_tracking") == 0) {
            faceTrackingSupported = true;
            enabledExtensions.push_back("XR_ANDROID_face_tracking");
        }
        if (strcmp(ext.extensionName, "XR_MND_headless") == 0) {
            enabledExtensions.push_back("XR_MND_headless");
        }
        if (strcmp(ext.extensionName, "XR_KHR_convert_timespec_time") == 0) {
            timespecSupported = true;
            enabledExtensions.push_back("XR_KHR_convert_timespec_time");
        }
    }

    // 2. Create Instance
    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    strncpy(createInfo.applicationInfo.applicationName, "Gabble", XR_MAX_APPLICATION_NAME_SIZE);
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
    createInfo.enabledExtensionCount = (uint32_t)enabledExtensions.size();
    createInfo.enabledExtensionNames = enabledExtensions.data();

    XrInstance instance;
    CHK_XR(xrCreateInstance(&createInfo, &instance));

    // Load function pointers
    if (faceTrackingSupported) {
        xrGetInstanceProcAddr(instance, "xrCreateFaceTrackerANDROID", (PFN_xrVoidFunction*)&xrCreateFaceTrackerANDROID_ptr);
        xrGetInstanceProcAddr(instance, "xrDestroyFaceTrackerANDROID", (PFN_xrVoidFunction*)&xrDestroyFaceTrackerANDROID_ptr);
        xrGetInstanceProcAddr(instance, "xrGetFaceStateANDROID", (PFN_xrVoidFunction*)&xrGetFaceStateANDROID_ptr);
    }
    if (timespecSupported) {
        xrGetInstanceProcAddr(instance, "xrConvertTimespecTimeToTimeKHR", (PFN_xrVoidFunction*)&xrConvertTimespecTimeToTimeKHR_ptr);
    }

    // 3. Get System
    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrSystemId systemId;
    CHK_XR(xrGetSystem(instance, &systemInfo, &systemId));

    // 4. Create Session
    XrSessionCreateInfo sessionCreateInfo{XR_TYPE_SESSION_CREATE_INFO};
    sessionCreateInfo.systemId = systemId;
    XrSession session;
    CHK_XR(xrCreateSession(instance, &sessionCreateInfo, &session));

    // 5. Create Face Tracker
    XrFaceTrackerANDROID faceTracker = XR_NULL_HANDLE;
    if (faceTrackingSupported && xrCreateFaceTrackerANDROID_ptr) {
        XrFaceTrackerCreateInfoANDROID faceTrackerCreateInfo{XR_TYPE_FACE_TRACKER_CREATE_INFO_ANDROID};
        CHK_XR(xrCreateFaceTrackerANDROID_ptr(session, &faceTrackerCreateInfo, &faceTracker));
    }

    // 6. Begin Session
    XrSessionBeginInfo beginInfo{XR_TYPE_SESSION_BEGIN_INFO};
    beginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    CHK_XR(xrBeginSession(session, &beginInfo));
    std::cout << "Broadcasting VRCFT Unified Expressions (v2) to Resonite (127.0.0.1:8888)..." << std::endl;

    // 7. Event/Frame Loop
    bool running = true;
    while (running) {
        XrEventDataBuffer event{XR_TYPE_EVENT_DATA_BUFFER};
        while (xrPollEvent(instance, &event) == XR_SUCCESS) {
            if (event.type == XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED) {
                XrEventDataSessionStateChanged* stateEvent = (XrEventDataSessionStateChanged*)&event;
                if (stateEvent->state == XR_SESSION_STATE_STOPPING || stateEvent->state == XR_SESSION_STATE_EXITING) {
                    running = false;
                }
            }
            event.type = XR_TYPE_EVENT_DATA_BUFFER;
        }

        if (!running) break;

        XrTime xrTime = 0;
        if (timespecSupported && xrConvertTimespecTimeToTimeKHR_ptr) {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            xrConvertTimespecTimeToTimeKHR_ptr(instance, &ts, &xrTime);
        }

        if (faceTracker != XR_NULL_HANDLE && xrGetFaceStateANDROID_ptr && xrTime != 0) {
            float parameters[XR_FACE_PARAMETER_COUNT_ANDROID];
            float regionConfidences[XR_FACE_REGION_CONFIDENCE_COUNT_ANDROID];
            
            XrFaceStateANDROID faceState{XR_TYPE_FACE_STATE_ANDROID};
            faceState.parametersCapacityInput = XR_FACE_PARAMETER_COUNT_ANDROID;
            faceState.parameters = parameters;
            faceState.regionConfidencesCapacityInput = XR_FACE_REGION_CONFIDENCE_COUNT_ANDROID;
            faceState.regionConfidences = regionConfidences;

            XrFaceStateGetInfoANDROID getInfo{XR_TYPE_FACE_STATE_GET_INFO_ANDROID};
            getInfo.time = xrTime;

            XrResult res = xrGetFaceStateANDROID_ptr(faceTracker, &getInfo, &faceState);
            if (res == XR_SUCCESS && faceState.isValid) {
                send_v2_parameters(resoniteAddr, parameters);
                std::cout << "\r[OSC Sent] JawOpen: " << std::fixed << std::setprecision(2) << parameters[24] << "    " << std::flush;
            }
        }

        struct timespec ts = {0, 10000000}; // 10ms (100Hz)
        nanosleep(&ts, NULL);
    }

    // Cleanup
    if (faceTracker != XR_NULL_HANDLE) xrDestroyFaceTrackerANDROID_ptr(faceTracker);
    xrEndSession(session);
    xrDestroySession(session);
    xrDestroyInstance(instance);
    lo_address_free(resoniteAddr);

    return 0;
}
