#include "FTUtils.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <cstdlib>
#include <vector>

FTUtils::FTUtils() {}
FTUtils::~FTUtils() {}

void FTUtils::LoadSensitivity() {
    const char* home = std::getenv("HOME");
    if (!home) return;

    std::filesystem::path configDir = std::filesystem::path(home) / ".config" / "gabble";
    std::filesystem::path configPath = configDir / "sensitivity.ini";

    // Create default file if it doesn't exist
    if (!std::filesystem::exists(configPath)) {
        std::cout << "Sensitivity config not found. Creating default at " << configPath << std::endl;
        try {
            std::filesystem::create_directories(configDir);
            std::ofstream outFile(configPath);
            if (outFile.is_open()) {
                outFile << "; Gabble Tracking Sensitivity Configuration\n\n"
                        << "EyeLid = 1.0\nEyeSquint = 1.0\nEyeWiden = 1.0\n"
                        << "BrowInnerUp = 1.0\nBrowOuterUp = 1.0\nBrowDown = 1.0\n"
                        << "CheekPuff = 1.0\nCheekSuck = 1.0\nCheekRaiser = 1.0\n"
                        << "JawOpen = 1.0\nMouthApeShape = 1.0\nJawX = 10.0\n"
                        << "JawForward = 1.0\nLipPucker = 1.0\nMouthX = 1.0\n"
                        << "MouthSmile = 1.0\nMouthFrown = 1.0\n"
                        << "LipFunnelTop = 1.0\nLipFunnelBottom = 1.0\n"
                        << "LipSuckTop = 1.0\nLipSuckBottom = 1.0\n"
                        << "ChinRaiserTop = 1.0\nChinRaiserBottom = 1.0\n"
                        << "MouthLowerDown = 1.0\nMouthUpperUp = 1.0\n"
                        << "MouthDimpler = 1.0\nMouthStretch = 1.0\n"
                        << "MouthPress = 1.0\nMouthTightener = 1.0\n"
                        << "NoseSneer = 10.0\n\n"
                        << "; Eyes extension\n"
                        << "EyeLeftLookLeft = 1.0\nEyeLeftLookRight = 1.0\n"
                        << "EyeLeftLookUp = 1.0\nEyeLeftLookDown = 1.0\n"
                        << "EyeLeftOpenness = 1.0\n"
                        << "EyeRightLookLeft = 1.0\nEyeRightLookRight = 1.0\n"
                        << "EyeRightLookUp = 1.0\nEyeRightLookDown = 1.0\n"
                        << "EyeRightOpenness = 1.0\n";
                outFile.close();
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to create default config: " << e.what() << std::endl;
        }
    }

    std::ifstream file(configPath);
    if (!file.is_open()) return;

    std::unordered_map<std::string, float*> mapping = {
        {"EyeLid", &sensitivity.EyeLid},
        {"EyeSquint", &sensitivity.EyeSquint},
        {"EyeWiden", &sensitivity.EyeWiden},
        {"BrowInnerUp", &sensitivity.BrowInnerUp},
        {"BrowOuterUp", &sensitivity.BrowOuterUp},
        {"BrowDown", &sensitivity.BrowDown},
        {"CheekPuff", &sensitivity.CheekPuff},
        {"CheekSuck", &sensitivity.CheekSuck},
        {"CheekRaiser", &sensitivity.CheekRaiser},
        {"JawOpen", &sensitivity.JawOpen},
        {"MouthApeShape", &sensitivity.MouthApeShape},
        {"JawX", &sensitivity.JawX},
        {"JawForward", &sensitivity.JawForward},
        {"LipPucker", &sensitivity.LipPucker},
        {"MouthX", &sensitivity.MouthX},
        {"MouthSmile", &sensitivity.MouthSmile},
        {"MouthFrown", &sensitivity.MouthFrown},
        {"LipFunnelTop", &sensitivity.LipFunnelTop},
        {"LipFunnelBottom", &sensitivity.LipFunnelBottom},
        {"LipSuckTop", &sensitivity.LipSuckTop},
        {"LipSuckBottom", &sensitivity.LipSuckBottom},
        {"ChinRaiserTop", &sensitivity.ChinRaiserTop},
        {"ChinRaiserBottom", &sensitivity.ChinRaiserBottom},
        {"MouthLowerDown", &sensitivity.MouthLowerDown},
        {"MouthUpperUp", &sensitivity.MouthUpperUp},
        {"MouthDimpler", &sensitivity.MouthDimpler},
        {"MouthStretch", &sensitivity.MouthStretch},
        {"MouthPress", &sensitivity.MouthPress},
        {"MouthTightener", &sensitivity.MouthTightener},
        {"NoseSneer", &sensitivity.NoseSneer},
        {"EyeLeftLookLeft", &sensitivity.EyeLeftLookLeft},
        {"EyeLeftLookRight", &sensitivity.EyeLeftLookRight},
        {"EyeLeftLookUp", &sensitivity.EyeLeftLookUp},
        {"EyeLeftLookDown", &sensitivity.EyeLeftLookDown},
        {"EyeLeftOpenness", &sensitivity.EyeLeftOpenness},
        {"EyeRightLookLeft", &sensitivity.EyeRightLookLeft},
        {"EyeRightLookRight", &sensitivity.EyeRightLookRight},
        {"EyeRightLookUp", &sensitivity.EyeRightLookUp},
        {"EyeRightLookDown", &sensitivity.EyeRightLookDown},
        {"EyeRightOpenness", &sensitivity.EyeRightOpenness}
    };

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';' || line[0] == '#' || line[0] == '[') continue;

        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string valueStr = line.substr(pos + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);

        valueStr.erase(0, valueStr.find_first_not_of(" \t"));
        valueStr.erase(valueStr.find_last_not_of(" \t") + 1);

        if (mapping.contains(key)) {
            try {
                *mapping[key] = std::stof(valueStr);
            } catch (...) {}
        }
    }
}
