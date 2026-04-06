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

const char* babbleNames[] = {
    "cheekPuffLeft", "cheekPuffRight", "cheekSuckLeft", "cheekSuckRight",
    "jawOpen", "jawForward", "jawLeft", "jawRight",
    "noseSneerLeft", "noseSneerRight", "mouthFunnel", "mouthPucker",
    "mouthLeft", "mouthRight", "mouthRollUpper", "mouthRollLower",
    "mouthShrugUpper", "mouthShrugLower", "mouthClose", "mouthSmileLeft",
    "mouthSmileRight", "mouthFrownLeft", "mouthFrownRight", "mouthDimpleLeft",
    "mouthDimpleRight", "mouthUpperUpLeft", "mouthUpperUpRight", "mouthLowerDownLeft",
    "mouthLowerDownRight", "mouthPressLeft", "mouthPressRight", "mouthStretchLeft",
    "mouthStretchRight", "tongueOut", "tongueUp", "tongueDown",
    "tongueLeft", "tongueRight", "tongueRoll", "tongueBendDown",
    "tongueCurlUp", "tongueSquish", "tongueFlat", "tongueTwistLeft",
    "tongueTwistRight"
};

void send_babble_parameters(lo_address addr, float* p) {
    float b[45];
    memset(b, 0, sizeof(b));

    // Mapping from XR_ANDROID_face_tracking (p) to Babble (b)
    b[0] = p[2];  // cheekPuffLeft
    b[1] = p[3];  // cheekPuffRight
    b[2] = p[6];  // cheekSuckLeft
    b[3] = p[7];  // cheekSuckRight
    b[4] = p[24]; // jawOpen
    b[5] = p[27]; // jawForward (Thrust)
    b[6] = p[25]; // jawLeft
    b[7] = p[26]; // jawRight
    b[8] = p[55]; // noseSneerLeft
    b[9] = p[56]; // noseSneerRight
    b[10] = (p[34] + p[35] + p[36] + p[37]) / 4.0f; // mouthFunnel
    b[11] = (p[40] + p[41]) / 2.0f;                 // mouthPucker
    b[12] = p[53]; // mouthLeft
    b[13] = p[54]; // mouthRight
    b[14] = (p[45] + p[47]) / 2.0f;                 // mouthRollUpper (Suck)
    b[15] = (p[44] + p[46]) / 2.0f;                 // mouthRollLower (Suck)
    b[16] = p[9];  // mouthShrugUpper (Chin Raiser Top)
    b[17] = p[8];  // mouthShrugLower (Chin Raiser Bottom)
    b[18] = p[50]; // mouthClose
    b[19] = p[32]; // mouthSmileLeft (Corner Puller)
    b[20] = p[33]; // mouthSmileRight (Corner Puller)
    b[21] = p[30]; // mouthFrownLeft (Corner Depressor)
    b[22] = p[31]; // mouthFrownRight (Corner Depressor)
    b[23] = p[10]; // mouthDimpleLeft
    b[24] = p[11]; // mouthDimpleRight
    b[25] = p[61]; // mouthUpperUpLeft (Raiser)
    b[26] = p[62]; // mouthUpperUpRight (Raiser)
    b[27] = p[51]; // mouthLowerDownLeft (Depressor)
    b[28] = p[52]; // mouthLowerDownRight (Depressor)
    b[29] = p[38]; // mouthPressLeft
    b[30] = p[39]; // mouthPressRight
    b[31] = p[42]; // mouthStretchLeft
    b[32] = p[43]; // mouthStretchRight
    b[33] = p[63]; // tongueOut
    b[34] = p[66]; // tongueUp
    b[35] = p[67]; // tongueDown
    b[36] = p[64]; // tongueLeft
    b[37] = p[65]; // tongueRight
    // 38-44 (tongueRoll, bend, curl, squish, flat, twists) - Android doesn't have these specifically

    for (int i = 0; i < 45; ++i) {
        std::string path = "/" + std::string(babbleNames[i]);
        lo_send(addr, path.c_str(), "f", b[i]);
    }
}

int main() {
    lo_address resoniteAddr = lo_address_new("127.0.0.1", "8888");
    if (!resoniteAddr) {
        std::cerr << "Failed to initialize OSC address" << std::endl;
        return 1;
    }

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

    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    strncpy(createInfo.applicationInfo.applicationName, "Gabble", XR_MAX_APPLICATION_NAME_SIZE);
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
    createInfo.enabledExtensionCount = (uint32_t)enabledExtensions.size();
    createInfo.enabledExtensionNames = enabledExtensions.data();

    XrInstance instance;
    CHK_XR(xrCreateInstance(&createInfo, &instance));

    if (faceTrackingSupported) {
        xrGetInstanceProcAddr(instance, "xrCreateFaceTrackerANDROID", (PFN_xrVoidFunction*)&xrCreateFaceTrackerANDROID_ptr);
        xrGetInstanceProcAddr(instance, "xrDestroyFaceTrackerANDROID", (PFN_xrVoidFunction*)&xrDestroyFaceTrackerANDROID_ptr);
        xrGetInstanceProcAddr(instance, "xrGetFaceStateANDROID", (PFN_xrVoidFunction*)&xrGetFaceStateANDROID_ptr);
    }
    if (timespecSupported) {
        xrGetInstanceProcAddr(instance, "xrConvertTimespecTimeToTimeKHR", (PFN_xrVoidFunction*)&xrConvertTimespecTimeToTimeKHR_ptr);
    }

    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrSystemId systemId;
    CHK_XR(xrGetSystem(instance, &systemInfo, &systemId));

    XrSessionCreateInfo sessionCreateInfo{XR_TYPE_SESSION_CREATE_INFO};
    sessionCreateInfo.systemId = systemId;
    XrSession session;
    CHK_XR(xrCreateSession(instance, &sessionCreateInfo, &session));

    XrFaceTrackerANDROID faceTracker = XR_NULL_HANDLE;
    if (faceTrackingSupported && xrCreateFaceTrackerANDROID_ptr) {
        XrFaceTrackerCreateInfoANDROID faceTrackerCreateInfo{XR_TYPE_FACE_TRACKER_CREATE_INFO_ANDROID};
        CHK_XR(xrCreateFaceTrackerANDROID_ptr(session, &faceTrackerCreateInfo, &faceTracker));
    }

    XrSessionBeginInfo beginInfo{XR_TYPE_SESSION_BEGIN_INFO};
    beginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    CHK_XR(xrBeginSession(session, &beginInfo));
    std::cout << "Broadcasting Babble OSC to Resonite (127.0.0.1:8888)..." << std::endl;

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
                send_babble_parameters(resoniteAddr, parameters);
                std::cout << "\r[Babble Sent] JawOpen: " << std::fixed << std::setprecision(2) << parameters[24] << "    " << std::flush;
            }
        }

        struct timespec ts = {0, 10000000}; // 10ms (100Hz)
        nanosleep(&ts, NULL);
    }

    if (faceTracker != XR_NULL_HANDLE) xrDestroyFaceTrackerANDROID_ptr(faceTracker);
    xrEndSession(session);
    xrDestroySession(session);
    xrDestroyInstance(instance);
    lo_address_free(resoniteAddr);

    return 0;
}
