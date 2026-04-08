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

void send_osc_data(lo_address addr, float* p) {
    // 1. Babble Expressions (Raw)
    lo_send(addr, "/cheekPuffLeft", "f", p[2]);
    lo_send(addr, "/cheekPuffRight", "f", p[3]);
    lo_send(addr, "/cheekSuckLeft", "f", p[6]);
    lo_send(addr, "/cheekSuckRight", "f", p[7]);
    lo_send(addr, "/jawOpen", "f", p[24]);
    lo_send(addr, "/jawForward", "f", p[27]);
    lo_send(addr, "/jawLeft", "f", p[25]);
    lo_send(addr, "/jawRight", "f", p[26]);
    lo_send(addr, "/noseSneerLeft", "f", p[55]);
    lo_send(addr, "/noseSneerRight", "f", p[56]);
    lo_send(addr, "/mouthFunnel", "f", (p[34] + p[35] + p[36] + p[37]) / 4.0f);
    lo_send(addr, "/mouthPucker", "f", (p[40] + p[41]) / 2.0f);
    lo_send(addr, "/mouthLeft", "f", p[53]);
    lo_send(addr, "/mouthRight", "f", p[54]);
    lo_send(addr, "/mouthRollUpper", "f", (p[45] + p[47]) / 2.0f);
    lo_send(addr, "/mouthRollLower", "f", (p[44] + p[46]) / 2.0f);
    lo_send(addr, "/mouthShrugUpper", "f", p[9]);
    lo_send(addr, "/mouthShrugLower", "f", p[8]);
    lo_send(addr, "/mouthClose", "f", p[50]);
    lo_send(addr, "/mouthSmileLeft", "f", p[32]);
    lo_send(addr, "/mouthSmileRight", "f", p[33]);
    lo_send(addr, "/mouthFrownLeft", "f", p[30]);
    lo_send(addr, "/mouthFrownRight", "f", p[31]);
    lo_send(addr, "/mouthDimpleLeft", "f", p[10]);
    lo_send(addr, "/mouthDimpleRight", "f", p[11]);
    lo_send(addr, "/mouthUpperUpLeft", "f", p[61]);
    lo_send(addr, "/mouthUpperUpRight", "f", p[62]);
    lo_send(addr, "/mouthLowerDownLeft", "f", p[51]);
    lo_send(addr, "/mouthLowerDownRight", "f", p[52]);
    lo_send(addr, "/mouthStretchLeft", "f", p[42]);
    lo_send(addr, "/mouthStretchRight", "f", p[43]);
    lo_send(addr, "/tongueOut", "f", p[63]);
    lo_send(addr, "/tongueUp", "f", p[66]);
    lo_send(addr, "/tongueDown", "f", p[67]);
    lo_send(addr, "/tongueLeft", "f", p[64]);
    lo_send(addr, "/tongueRight", "f", p[65]);
    // Tongue roll/bend/etc (Android N/A)
    lo_send(addr, "/tongueRoll", "f", 0.0f);
    lo_send(addr, "/tongueBendDown", "f", 0.0f);
    lo_send(addr, "/tongueCurlUp", "f", 0.0f);
    lo_send(addr, "/tongueSquish", "f", 0.0f);
    lo_send(addr, "/tongueFlat", "f", 0.0f);
    lo_send(addr, "/tongueTwistLeft", "f", 0.0f);
    lo_send(addr, "/tongueTwistRight", "f", 0.0f);
    lo_send(addr, "/mouthPressLeft", "f", p[38]);
    lo_send(addr, "/mouthPressRight", "f", p[39]);

    // 2. Eye Gaze & Lids
    // LeftEyeX/RightEyeX: Right - Left
    float eyeLX = p[18] - p[16];
    float eyeRX = p[19] - p[17];
    float eyeLY = p[20] - p[14];
    float eyeRY = p[21] - p[15];
    float eyeY = (eyeLY + eyeRY) / 2.0f;

    // Lids: 1.0 is open, 0.0 is closed. Android: Closed=1.0.
    float lidL = 1.0f - p[12] + p[59];
    float lidR = 1.0f - p[13] + p[60];

    lo_send(addr, "/LeftEyeX", "f", eyeLX * 10.0);
    lo_send(addr, "/RightEyeX", "f", eyeRX * 10.0);
    lo_send(addr, "/EyesY", "f", eyeY * 10.0);
    lo_send(addr, "/LeftEyeLid", "f", lidL * 10.0);
    lo_send(addr, "/RightEyeLid", "f", lidR * 10.0);

    // std::cout << "Eye data:\n";
    // std::cout << "LeftEyeX: " << eyeLX * 10.0 << "\n" <<
    // "RightEyeX: " << eyeRX * 10.0 <<"\n" <<
    // "EyesY: " << eyeY * 10.0 <<"\n" <<
    // "LeftEyeLid: " << lidL * 10.0 << "\n" <<
    // "RightEyeLid: " << lidR * 10.0 << "\n";

    // V2 Eye Parameters
    // lo_send(addr, "/v2/EyeLeftX", "f", eyeLX);
    // lo_send(addr, "/v2/EyeRightX", "f", eyeRX);
    // lo_send(addr, "/v2/EyeLeftY", "f", eyeLY);
    
    // // V2 Lids: 0.75 is standard open.
    // float v2LidL = (1.0f - p[12]) * 0.75f + p[59] * 0.25f;
    // float v2LidR = (1.0f - p[13]) * 0.75f + p[60] * 0.25f;
    // lo_send(addr, "/v2/EyeLidLeft", "f", v2LidL);
    // lo_send(addr, "/v2/EyeLidRight", "f", v2LidR);
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
    std::cout << "Broadcasting Babble + Eye OSC to Resonite (127.0.0.1:8888)..." << std::endl;

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
                send_osc_data(resoniteAddr, parameters);
                std::cout << "\r[OSC] Data sent to 8888.    " << std::flush;
            }
        }

        struct timespec ts = {0, 10000000}; // 10ms
        nanosleep(&ts, NULL);
    }

    if (faceTracker != XR_NULL_HANDLE) xrDestroyFaceTrackerANDROID_ptr(faceTracker);
    xrEndSession(session);
    xrDestroySession(session);
    xrDestroyInstance(instance);
    lo_address_free(resoniteAddr);

    return 0;
}
