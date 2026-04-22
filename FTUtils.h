#pragma once

#include <array>
#include <cstdint>
#include <algorithm>
#include <span>
#include <vector>
#include <string>
#include "OneEuroFilter.h"
#include <openxr/openxr.h>

class FTUtils
{
public:
    FTUtils();
    ~FTUtils();

    static void LoadSensitivity();

    static std::array<float, 2> MakeEye(float LookLeft, float LookRight, float LookUp, float LookDown)
    {
        float h = (LookRight - LookLeft) * 0.5f;
        float v = (LookUp - LookDown) * 0.5f;
        return {h, v};
    }
};

/**
 * @brief Container of information pertaining to a singular Unified Expression shape.
 */
struct UnifiedExpressionShape
{
    /**
     * @brief Value that contains the specified Unified Expression raw value.
     */
    float weight = 0.0f;
};

enum class UnifiedExpressions : uint32_t
{
    // Eye Expressions
    EyeSquintRight,
    EyeSquintLeft,
    EyeWideRight,
    EyeWideLeft,

    // Eyebrow Expressions
    BrowPinchRight,
    BrowPinchLeft,
    BrowLowererRight,
    BrowLowererLeft,
    BrowInnerUpRight,
    BrowInnerUpLeft,
    BrowOuterUpRight,
    BrowOuterUpLeft,

    // Nose Expressions
    NasalDilationRight,
    NasalDilationLeft,
    NasalConstrictRight,
    NasalConstrictLeft,

    // Cheek Expressions
    CheekSquintRight,
    CheekSquintLeft,
    CheekPuffRight,
    CheekPuffLeft,
    CheekSuckRight,
    CheekSuckLeft,

    // Jaw Exclusive Expressions
    JawOpen,
    JawRight,
    JawLeft,
    JawForward,
    JawBackward,
    JawClench,
    JawMandibleRaise,
    MouthClosed,

    // Lip Expressions
    LipSuckUpperRight,
    LipSuckUpperLeft,
    LipSuckLowerRight,
    LipSuckLowerLeft,
    LipSuckCornerRight,
    LipSuckCornerLeft,
    LipFunnelUpperRight,
    LipFunnelUpperLeft,
    LipFunnelLowerRight,
    LipFunnelLowerLeft,
    LipPuckerUpperRight,
    LipPuckerUpperLeft,
    LipPuckerLowerRight,
    LipPuckerLowerLeft,
    MouthUpperUpRight,
    MouthUpperUpLeft,
    MouthUpperDeepenRight,
    MouthUpperDeepenLeft,
    NoseSneerRight,
    NoseSneerLeft,
    MouthLowerDownRight,
    MouthLowerDownLeft,
    MouthUpperRight,
    MouthUpperLeft,
    MouthLowerRight,
    MouthLowerLeft,
    MouthCornerPullRight,
    MouthCornerPullLeft,
    MouthCornerSlantRight,
    MouthCornerSlantLeft,
    MouthFrownRight,
    MouthFrownLeft,
    MouthStretchRight,
    MouthStretchLeft,
    MouthDimpleRight,
    MouthDimpleLeft,
    MouthRaiserUpper,
    MouthRaiserLower,
    MouthPressRight,
    MouthPressLeft,
    MouthTightenerRight,
    MouthTightenerLeft,

    // Tongue Expressions
    TongueOut,
    TongueUp,
    TongueDown,
    TongueRight,
    TongueLeft,
    TongueRoll,
    TongueBendDown,
    TongueCurlUp,
    TongueSquish,
    TongueFlat,
    TongueTwistRight,
    TongueTwistLeft,

    // Throat/Neck Expressions
    SoftPalateClose,
    ThroatSwallow,
    NeckFlexRight,
    NeckFlexLeft,

    // Eyes extension
    EyeLeftOpenness,
    EyeLeftLookLeft,
    EyeLeftLookRight,
    EyeLeftLookUp,
    EyeLeftLookDown,

    EyeRightOpenness,
    EyeRightLookLeft,
    EyeRightLookRight,
    EyeRightLookUp,
    EyeRightLookDown,

    Max
};

struct TrackingSensitivity
{
    // Tracking Sensitivity Multipliers
    float EyeLid = 1.0f;
    float EyeSquint = 1.0f;
    float EyeWiden = 1.0f;
    float BrowInnerUp = 1.0f;
    float BrowOuterUp = 1.0f;
    float BrowDown = 1.0f;
    float CheekPuff = 1.0f;
    float CheekSuck = 1.0f;
    float CheekRaiser = 1.0f;
    float JawOpen = 1.0f;
    float MouthApeShape = 1.0f;
    float JawX = 10.0f;
    float JawForward = 1.0f;
    float LipPucker = 1.0f;
    float MouthX = 1.0f;
    float MouthSmile = 1.0f;
    float MouthFrown = 1.0f;
    float LipFunnelTop = 1.0f;
    float LipFunnelBottom = 1.0f;
    float LipSuckTop = 1.0f;
    float LipSuckBottom = 1.0f;
    float ChinRaiserTop = 1.0f;
    float ChinRaiserBottom = 1.0f;
    float MouthLowerDown = 1.0f;
    float MouthUpperUp = 1.0f;
    float MouthDimpler = 1.0f;
    float MouthStretch = 1.0f;
    float MouthPress = 1.0f;
    float MouthTightener = 1.0f;
    float NoseSneer = 10.0f;

    // Eyes extension
    float EyeLeftLookLeft = 1.0f;
    float EyeLeftLookRight = 1.0f;
    float EyeLeftLookUp = 1.0f;
    float EyeLeftLookDown = 1.0f;
    float EyeLeftOpenness = 1.0f;
    float EyeRightLookLeft = 1.0f;
    float EyeRightLookRight = 1.0f;
    float EyeRightLookUp = 1.0f;
    float EyeRightLookDown = 1.0f;
    float EyeRightOpenness = 1.0f;
};

inline TrackingSensitivity sensitivity;

/**
 * @brief Updates unified expressions based on Android input.
 * @param unifiedExpressions A span (reference) to the shapes array to modify.
 * @param expressions A read-only span of float input values.
 */
static void UpdateEyeExpressionsANDROID(
    std::span<UnifiedExpressionShape> unifiedExpressions,
    std::span<const float> expressions)
{
    // Extending UE with Eye Gaze
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeLeftLookLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_EYES_LOOK_LEFT_L_ANDROID] * sensitivity.EyeLeftLookLeft);
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeLeftLookRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_EYES_LOOK_RIGHT_L_ANDROID] * sensitivity.EyeLeftLookRight);
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeLeftLookUp].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_EYES_LOOK_UP_L_ANDROID] * sensitivity.EyeLeftLookUp);
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeLeftLookDown].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_EYES_LOOK_DOWN_L_ANDROID] * sensitivity.EyeLeftLookDown);
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeLeftOpenness].weight =
        std::min(1.0f, (1.0f - expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_EYES_CLOSED_L_ANDROID]) * sensitivity.EyeLeftOpenness);

    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeRightLookLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_EYES_LOOK_LEFT_R_ANDROID] * sensitivity.EyeRightLookLeft);
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeRightLookRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_EYES_LOOK_RIGHT_R_ANDROID] * sensitivity.EyeRightLookRight);
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeRightLookUp].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_EYES_LOOK_UP_R_ANDROID] * sensitivity.EyeRightLookUp);
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeRightLookDown].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_EYES_LOOK_DOWN_R_ANDROID] * sensitivity.EyeRightLookDown);
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeRightOpenness].weight =
        std::min(1.0f, (1.0f - expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_EYES_CLOSED_R_ANDROID]) * sensitivity.EyeRightOpenness);

    // Eye Expressions Set
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeWideLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_UPPER_LID_RAISER_L_ANDROID] * sensitivity.EyeWiden);
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeWideRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_UPPER_LID_RAISER_R_ANDROID] * sensitivity.EyeWiden);

    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeSquintLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LID_TIGHTENER_L_ANDROID] * sensitivity.EyeSquint);
    unifiedExpressions[(uint32_t)UnifiedExpressions::EyeSquintRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LID_TIGHTENER_R_ANDROID] * sensitivity.EyeSquint);

    // Brow Expressions Set
    unifiedExpressions[(uint32_t)UnifiedExpressions::BrowInnerUpLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_INNER_BROW_RAISER_L_ANDROID] * sensitivity.BrowInnerUp);
    unifiedExpressions[(uint32_t)UnifiedExpressions::BrowInnerUpRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_INNER_BROW_RAISER_R_ANDROID] * sensitivity.BrowInnerUp);
    unifiedExpressions[(uint32_t)UnifiedExpressions::BrowOuterUpLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_OUTER_BROW_RAISER_L_ANDROID] * sensitivity.BrowOuterUp);
    unifiedExpressions[(uint32_t)UnifiedExpressions::BrowOuterUpRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_OUTER_BROW_RAISER_R_ANDROID] * sensitivity.BrowOuterUp);

    unifiedExpressions[(uint32_t)UnifiedExpressions::BrowLowererLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_BROW_LOWERER_L_ANDROID] * sensitivity.BrowDown);
    unifiedExpressions[(uint32_t)UnifiedExpressions::BrowPinchLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_BROW_LOWERER_L_ANDROID] * sensitivity.BrowDown);
    unifiedExpressions[(uint32_t)UnifiedExpressions::BrowLowererRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_BROW_LOWERER_R_ANDROID] * sensitivity.BrowDown);
    unifiedExpressions[(uint32_t)UnifiedExpressions::BrowPinchRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_BROW_LOWERER_R_ANDROID] * sensitivity.BrowDown);
}

/**
 * @brief Updates unified expressions based on Android input.
 * @param unifiedExpressions A span (reference) to the shapes array to modify.
 * @param expressions A read-only span of float input values.
 */
static void UpdateMouthExpressionsANDROID(
    std::span<UnifiedExpressionShape> unifiedExpressions,
    std::span<const float> expressions)
{
    // Jaw Expression Set
    unifiedExpressions[(uint32_t)UnifiedExpressions::JawOpen].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_JAW_DROP_ANDROID] * sensitivity.JawOpen);
    unifiedExpressions[(uint32_t)UnifiedExpressions::JawLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_JAW_SIDEWAYS_LEFT_ANDROID] * sensitivity.JawX);
    unifiedExpressions[(uint32_t)UnifiedExpressions::JawRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_JAW_SIDEWAYS_RIGHT_ANDROID] * sensitivity.JawX);
    unifiedExpressions[(uint32_t)UnifiedExpressions::JawForward].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_JAW_THRUST_ANDROID] * sensitivity.JawForward);

    // Mouth Expression Set
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthClosed].weight = expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIPS_TOWARD_ANDROID];

    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthUpperLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_MOUTH_LEFT_ANDROID] * sensitivity.MouthX);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthLowerLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_MOUTH_LEFT_ANDROID] * sensitivity.MouthX);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthUpperRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_MOUTH_RIGHT_ANDROID] * sensitivity.MouthX);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthLowerRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_MOUTH_RIGHT_ANDROID] * sensitivity.MouthX);

    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthCornerPullLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_CORNER_PULLER_L_ANDROID] * sensitivity.MouthSmile);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthCornerSlantLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_CORNER_PULLER_L_ANDROID] * sensitivity.MouthSmile);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthCornerPullRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_CORNER_PULLER_R_ANDROID] * sensitivity.MouthSmile);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthCornerSlantRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_CORNER_PULLER_R_ANDROID] * sensitivity.MouthSmile);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthFrownLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_CORNER_DEPRESSOR_L_ANDROID] * sensitivity.MouthFrown);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthFrownRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_CORNER_DEPRESSOR_R_ANDROID] * sensitivity.MouthFrown);

    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthLowerDownLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LOWER_LIP_DEPRESSOR_L_ANDROID] * sensitivity.MouthLowerDown);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthLowerDownRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LOWER_LIP_DEPRESSOR_R_ANDROID] * sensitivity.MouthLowerDown);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthUpperUpLeft].weight =
        std::min(1.0f, sensitivity.MouthUpperUp * std::max(expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_UPPER_LIP_RAISER_L_ANDROID], expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_NOSE_WRINKLER_L_ANDROID])); // Workaround for wierd tracking quirk.
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthUpperDeepenLeft].weight =
        std::min(1.0f, sensitivity.MouthUpperUp * std::max(expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_UPPER_LIP_RAISER_L_ANDROID], expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_NOSE_WRINKLER_L_ANDROID])); // Workaround for wierd tracking quirk.
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthUpperUpRight].weight =
        std::min(1.0f, sensitivity.MouthUpperUp * std::max(expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_UPPER_LIP_RAISER_R_ANDROID], expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_NOSE_WRINKLER_R_ANDROID])); // Workaround for wierd tracking quirk.
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthUpperDeepenRight].weight =
        std::min(1.0f, sensitivity.MouthUpperUp * std::max(expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_UPPER_LIP_RAISER_R_ANDROID], expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_NOSE_WRINKLER_R_ANDROID])); // Workaround for wierd tracking quirk.

    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthRaiserUpper].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_CHIN_RAISER_T_ANDROID] * sensitivity.ChinRaiserTop);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthRaiserLower].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_CHIN_RAISER_B_ANDROID] * sensitivity.ChinRaiserBottom);

    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthDimpleLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_DIMPLER_L_ANDROID] * sensitivity.MouthDimpler);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthDimpleRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_DIMPLER_R_ANDROID] * sensitivity.MouthDimpler);

    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthTightenerLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_TIGHTENER_L_ANDROID] * sensitivity.MouthTightener);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthTightenerRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_TIGHTENER_R_ANDROID] * sensitivity.MouthTightener);

    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthPressLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_PRESSOR_L_ANDROID] * sensitivity.MouthPress);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthPressRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_PRESSOR_R_ANDROID] * sensitivity.MouthPress);

    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthStretchLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_STRETCHER_L_ANDROID] * sensitivity.MouthStretch);
    unifiedExpressions[(uint32_t)UnifiedExpressions::MouthStretchRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_STRETCHER_R_ANDROID] * sensitivity.MouthStretch);

    // Lip Expression Set
    unifiedExpressions[(uint32_t)UnifiedExpressions::LipPuckerUpperRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_PUCKER_R_ANDROID] * sensitivity.LipPucker);
    unifiedExpressions[(uint32_t)UnifiedExpressions::LipPuckerLowerRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_PUCKER_R_ANDROID] * sensitivity.LipPucker);
    unifiedExpressions[(uint32_t)UnifiedExpressions::LipPuckerUpperLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_PUCKER_L_ANDROID] * sensitivity.LipPucker);
    unifiedExpressions[(uint32_t)UnifiedExpressions::LipPuckerLowerLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_PUCKER_L_ANDROID] * sensitivity.LipPucker);

    unifiedExpressions[(uint32_t)UnifiedExpressions::LipFunnelUpperLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_FUNNELER_LT_ANDROID] * sensitivity.LipFunnelTop);
    unifiedExpressions[(uint32_t)UnifiedExpressions::LipFunnelUpperRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_FUNNELER_RT_ANDROID] * sensitivity.LipFunnelTop);
    unifiedExpressions[(uint32_t)UnifiedExpressions::LipFunnelLowerLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_FUNNELER_LB_ANDROID] * sensitivity.LipFunnelBottom);
    unifiedExpressions[(uint32_t)UnifiedExpressions::LipFunnelLowerRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_FUNNELER_RB_ANDROID] * sensitivity.LipFunnelBottom);

    unifiedExpressions[(uint32_t)UnifiedExpressions::LipSuckUpperLeft].weight =
        std::min(1.0f, sensitivity.LipSuckTop * std::min(1.0f - (float)std::powf(expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_UPPER_LIP_RAISER_L_ANDROID], 1.0f / 6.0f), expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_SUCK_LT_ANDROID]));
    unifiedExpressions[(uint32_t)UnifiedExpressions::LipSuckUpperRight].weight =
        std::min(1.0f, sensitivity.LipSuckTop * std::min(1.0f - (float)std::powf(expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_UPPER_LIP_RAISER_R_ANDROID], 1.0f / 6.0f), expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_SUCK_RT_ANDROID]));
    unifiedExpressions[(uint32_t)UnifiedExpressions::LipSuckLowerLeft].weight =
        std::min(1.0f, sensitivity.LipSuckBottom * expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_SUCK_LB_ANDROID]);
    unifiedExpressions[(uint32_t)UnifiedExpressions::LipSuckLowerRight].weight =
        std::min(1.0f, sensitivity.LipSuckBottom * expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_LIP_SUCK_RB_ANDROID]);

    // Cheek Expression Set
    unifiedExpressions[(uint32_t)UnifiedExpressions::CheekPuffLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_CHEEK_PUFF_L_ANDROID] * sensitivity.CheekPuff);
    unifiedExpressions[(uint32_t)UnifiedExpressions::CheekPuffRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_CHEEK_PUFF_R_ANDROID] * sensitivity.CheekPuff);
    unifiedExpressions[(uint32_t)UnifiedExpressions::CheekSuckLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_CHEEK_SUCK_L_ANDROID] * sensitivity.CheekSuck);
    unifiedExpressions[(uint32_t)UnifiedExpressions::CheekSuckRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_CHEEK_SUCK_R_ANDROID] * sensitivity.CheekSuck);
    unifiedExpressions[(uint32_t)UnifiedExpressions::CheekSquintLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_CHEEK_RAISER_L_ANDROID] * sensitivity.CheekRaiser);
    unifiedExpressions[(uint32_t)UnifiedExpressions::CheekSquintRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_CHEEK_RAISER_R_ANDROID] * sensitivity.CheekRaiser);

    // Nose Expression Set
    unifiedExpressions[(uint32_t)UnifiedExpressions::NoseSneerLeft].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_NOSE_WRINKLER_L_ANDROID] * sensitivity.NoseSneer);
    unifiedExpressions[(uint32_t)UnifiedExpressions::NoseSneerRight].weight =
        std::min(1.0f, expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_NOSE_WRINKLER_R_ANDROID] * sensitivity.NoseSneer);

    // Tongue Expression Set
    unifiedExpressions[(uint32_t)UnifiedExpressions::TongueOut].weight = expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_TONGUE_OUT_ANDROID];
    unifiedExpressions[(uint32_t)UnifiedExpressions::TongueLeft].weight = expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_TONGUE_LEFT_ANDROID];
    unifiedExpressions[(uint32_t)UnifiedExpressions::TongueRight].weight = expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_TONGUE_RIGHT_ANDROID];
    unifiedExpressions[(uint32_t)UnifiedExpressions::TongueUp].weight = expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_TONGUE_UP_ANDROID];
    unifiedExpressions[(uint32_t)UnifiedExpressions::TongueDown].weight = expressions[(int)XrFaceParameterIndicesANDROID::XR_FACE_PARAMETER_INDICES_TONGUE_DOWN_ANDROID];
};