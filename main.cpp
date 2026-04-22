#include <iostream>
#include <vector>
#include <string.h>
#include <ctime>
#include <iomanip>
#include <print>
#include <chrono>
#include <lo/lo.h>
#include "FTUtils.h"

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

void send_osc_data(lo_address addr, std::span<UnifiedExpressionShape> unifiedExpressions) {
    static auto last_log_time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    bool should_log = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_log_time).count() >= 100;

    auto send_f = [&](const char* path, float val) {
        lo_send(addr, path, "f", val);
        if (should_log) std::cout << path << ": " << std::setw(6) << val << " | ";
    };

    auto send_fff = [&](const char* path, float x, float y, float z) {
        lo_send(addr, path, "fff", x, y, z);
        if (should_log) std::cout << path << ": (" << x << "," << y << "," << z << ") | ";
    };

    if (should_log) {
        std::cout << "\033[H\033[J"; // Clear screen and home cursor
        std::cout << "--- OSC Parameters ---" << std::endl;
        std::cout << std::fixed << std::setprecision(3);
    }

    // 1. Mouth Properties
    float mouth_jaw_x = unifiedExpressions[(uint32_t)UnifiedExpressions::JawRight].weight - unifiedExpressions[(uint32_t)UnifiedExpressions::JawLeft].weight;
    float mouth_jaw_z = unifiedExpressions[(uint32_t)UnifiedExpressions::JawForward].weight;
    float mouth_jaw_open = unifiedExpressions[(uint32_t)UnifiedExpressions::JawOpen].weight;
    send_fff("/mouth/Jaw", mouth_jaw_x, 0.0f, mouth_jaw_z);
    send_f("/mouth/JawOpen", mouth_jaw_open);
    if (should_log) std::cout << std::endl;

    float tongue_x = unifiedExpressions[(uint32_t)UnifiedExpressions::TongueRight].weight - unifiedExpressions[(uint32_t)UnifiedExpressions::TongueLeft].weight;
    float tongue_y = unifiedExpressions[(uint32_t)UnifiedExpressions::TongueUp].weight - unifiedExpressions[(uint32_t)UnifiedExpressions::TongueDown].weight;
    float tongue_z = unifiedExpressions[(uint32_t)UnifiedExpressions::TongueOut].weight;
    send_fff("/mouth/Tongue", tongue_x, tongue_y, tongue_z);
    send_f("/mouth/TongueRoll", 0.0f);
    if (should_log) std::cout << std::endl;
    
    send_f("/mouth/LipUpperLeftRaise", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthUpperUpLeft].weight);
    send_f("/mouth/LipUpperRightRaise", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthUpperUpRight].weight);
    send_f("/mouth/LipLowerLeftRaise", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthLowerDownLeft].weight);
    send_f("/mouth/LipLowerRightRaise", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthLowerDownRight].weight);
    if (should_log) std::cout << std::endl;

    // float mouth_h = unifiedExpressions[XR_FACE_PARAMETER_INDICES_MOUTH_RIGHT_ANDROID] - unifiedExpressions[XR_FACE_PARAMETER_INDICES_MOUTH_LEFT_ANDROID];
    // send_f("/mouth/LipUpperHorizontal", mouth_h);
    // send_f("/mouth/LipLowerHorizontal", mouth_h);
    send_f("/mouth/MouthLeftSmileFrown", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthCornerPullLeft].weight - unifiedExpressions[(uint32_t)UnifiedExpressions::MouthFrownLeft].weight);
    send_f("/mouth/MouthRightSmileFrown", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthCornerPullRight].weight - unifiedExpressions[(uint32_t)UnifiedExpressions::MouthFrownRight].weight);
    if (should_log) std::cout << std::endl;

    send_f("/mouth/MouthLeftDimple", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthDimpleLeft].weight);
    send_f("/mouth/MouthRightDimple", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthDimpleRight].weight);
    // send_f("/mouth/MouthPoutLeft", unifiedExpressions[XR_FACE_PARAMETER_INDICES_LIP_PUCKER_L_ANDROID]);
    // send_f("/mouth/MouthPoutRight", unifiedExpressions[XR_FACE_PARAMETER_INDICES_LIP_PUCKER_R_ANDROID]);
    if (should_log) std::cout << std::endl;

    send_f("/mouth/LipTopLeftOverturn", unifiedExpressions[(uint32_t)UnifiedExpressions::LipFunnelUpperLeft].weight);
    send_f("/mouth/LipTopRightOverturn", unifiedExpressions[(uint32_t)UnifiedExpressions::LipFunnelUpperRight].weight);
    send_f("/mouth/LipBottomLeftOverturn", unifiedExpressions[(uint32_t)UnifiedExpressions::LipFunnelLowerLeft].weight);
    send_f("/mouth/LipBottomRightOverturn", unifiedExpressions[(uint32_t)UnifiedExpressions::LipFunnelLowerRight].weight);
    if (should_log) std::cout << std::endl;

    send_f("/mouth/LipTopLeftOverUnder", unifiedExpressions[(uint32_t)UnifiedExpressions::LipSuckUpperLeft].weight);
    send_f("/mouth/LipTopRightOverUnder", unifiedExpressions[(uint32_t)UnifiedExpressions::LipSuckUpperRight].weight);
    send_f("/mouth/LipBottomLeftOverUnder", unifiedExpressions[(uint32_t)UnifiedExpressions::LipSuckLowerLeft].weight);
    send_f("/mouth/LipBottomRightOverUnder", unifiedExpressions[(uint32_t)UnifiedExpressions::LipSuckLowerRight].weight);
    if (should_log) std::cout << std::endl;

    send_f("/mouth/LipLeftStretchTighten", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthStretchLeft].weight - unifiedExpressions[(uint32_t)UnifiedExpressions::MouthTightenerLeft].weight);
    send_f("/mouth/LipRightStretchTighten", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthStretchRight].weight - unifiedExpressions[(uint32_t)UnifiedExpressions::MouthTightenerRight].weight);
    send_f("/mouth/LipsLeftPress", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthPressLeft].weight);
    send_f("/mouth/LipsRightPress", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthPressRight].weight);
    if (should_log) std::cout << std::endl;

    send_f("/mouth/CheekLeftPuffSuck", unifiedExpressions[(uint32_t)UnifiedExpressions::CheekPuffLeft].weight - unifiedExpressions[(uint32_t)UnifiedExpressions::CheekSuckLeft].weight);
    send_f("/mouth/CheekRightPuffSuck", unifiedExpressions[(uint32_t)UnifiedExpressions::CheekPuffRight].weight - unifiedExpressions[(uint32_t)UnifiedExpressions::CheekSuckRight].weight);
    send_f("/mouth/CheekLeftRaise", unifiedExpressions[(uint32_t)UnifiedExpressions::CheekSquintLeft].weight);
    send_f("/mouth/CheekRightRaise", unifiedExpressions[(uint32_t)UnifiedExpressions::CheekSquintRight].weight);
    if (should_log) std::cout << std::endl;

    send_f("/mouth/NoseWrinkleLeft", unifiedExpressions[(uint32_t)UnifiedExpressions::NoseSneerLeft].weight);
    send_f("/mouth/NoseWrinkleRight", unifiedExpressions[(uint32_t)UnifiedExpressions::NoseSneerRight].weight);
    send_f("/mouth/ChinRaiseBottom", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthRaiserLower].weight);
    send_f("/mouth/ChinRaiseTop", unifiedExpressions[(uint32_t)UnifiedExpressions::MouthRaiserUpper].weight);
    if (should_log) std::cout << std::endl;

    // // 2. Eye Properties
    // // Left Eye
    // float eye_l_dir_x = unifiedExpressions[XR_FACE_PARAMETER_INDICES_EYES_LOOK_RIGHT_L_ANDROID] - unifiedExpressions[XR_FACE_PARAMETER_INDICES_EYES_LOOK_LEFT_L_ANDROID];
    // float eye_l_dir_y = unifiedExpressions[XR_FACE_PARAMETER_INDICES_EYES_LOOK_UP_L_ANDROID] - unifiedExpressions[XR_FACE_PARAMETER_INDICES_EYES_LOOK_DOWN_L_ANDROID];
    auto eye_l_gaze = FTUtils::MakeEye(
        unifiedExpressions[(uint32_t)UnifiedExpressions::EyeLeftLookLeft].weight,
        unifiedExpressions[(uint32_t)UnifiedExpressions::EyeLeftLookRight].weight,
        unifiedExpressions[(uint32_t)UnifiedExpressions::EyeLeftLookUp].weight,
        unifiedExpressions[(uint32_t)UnifiedExpressions::EyeLeftLookDown].weight
    );
    send_fff("/eye/Left/Direction", eye_l_gaze[0], eye_l_gaze[1], 0.0f);
    send_f("/eye/Left/Openness", unifiedExpressions[(uint32_t)UnifiedExpressions::EyeLeftOpenness].weight);
    send_f("/eye/Left/Widen", unifiedExpressions[(uint32_t)UnifiedExpressions::EyeWideLeft].weight);
    send_f("/eye/Left/Frown", unifiedExpressions[(uint32_t)UnifiedExpressions::BrowLowererLeft].weight);
    send_f("/eye/Left/Squeeze", unifiedExpressions[(uint32_t)UnifiedExpressions::EyeSquintLeft].weight);
    send_f("/eye/Left/InnerBrowVertical", unifiedExpressions[(uint32_t)UnifiedExpressions::BrowInnerUpLeft].weight);
    send_f("/eye/Left/OuterBrowVertical", unifiedExpressions[(uint32_t)UnifiedExpressions::BrowOuterUpLeft].weight);
    if (should_log) std::cout << std::endl;

    // // Right Eye
    // float eye_r_dir_x = unifiedExpressions[XR_FACE_PARAMETER_INDICES_EYES_LOOK_RIGHT_R_ANDROID] - unifiedExpressions[XR_FACE_PARAMETER_INDICES_EYES_LOOK_LEFT_R_ANDROID];
    // float eye_r_dir_y = unifiedExpressions[XR_FACE_PARAMETER_INDICES_EYES_LOOK_UP_R_ANDROID] - unifiedExpressions[XR_FACE_PARAMETER_INDICES_EYES_LOOK_DOWN_R_ANDROID];
    auto eye_r_gaze = FTUtils::MakeEye(
        unifiedExpressions[(uint32_t)UnifiedExpressions::EyeRightLookLeft].weight,
        unifiedExpressions[(uint32_t)UnifiedExpressions::EyeRightLookRight].weight,
        unifiedExpressions[(uint32_t)UnifiedExpressions::EyeRightLookUp].weight,
        unifiedExpressions[(uint32_t)UnifiedExpressions::EyeRightLookDown].weight
    );
    send_fff("/eye/Right/Direction", eye_r_gaze[0], eye_r_gaze[1], 0.0f);
    send_f("/eye/Right/Openness", unifiedExpressions[(uint32_t)UnifiedExpressions::EyeRightOpenness].weight);
    send_f("/eye/Right/Widen", unifiedExpressions[(uint32_t)UnifiedExpressions::EyeWideRight].weight);
    send_f("/eye/Right/Frown", unifiedExpressions[(uint32_t)UnifiedExpressions::BrowPinchRight].weight);
    send_f("/eye/Right/Squeeze", unifiedExpressions[(uint32_t)UnifiedExpressions::EyeSquintRight].weight);
    send_f("/eye/Right/InnerBrowVertical", unifiedExpressions[(uint32_t)UnifiedExpressions::BrowInnerUpRight].weight);
    send_f("/eye/Right/OuterBrowVertical", unifiedExpressions[(uint32_t)UnifiedExpressions::BrowOuterUpRight].weight);
    
    if (should_log) {
        std::cout << std::endl << std::flush;
        last_log_time = now;
    }
}

int main() {
    {
        std::print(stdout, "HELLO EASY PRINT\n");
    }
    
    lo_address resoniteAddr = lo_address_new("127.0.0.1", "9000");
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
    std::cout << "Broadcasting Babble + Eye OSC to Resonite (127.0.0.1:9000)..." << std::endl;

    // Init UExpressions
    std::array<UnifiedExpressionShape, static_cast<size_t>(UnifiedExpressions::Max)> unifiedExpressionsBuf;
    std::span<UnifiedExpressionShape> unifiedExpressions{unifiedExpressionsBuf};

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
                // Update UnifiedExpressions buffer
                UpdateEyeExpressionsANDROID(unifiedExpressions, parameters);
                UpdateMouthExpressionsANDROID(unifiedExpressions, parameters);

                // Send to socket
                send_osc_data(resoniteAddr, unifiedExpressions);
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
