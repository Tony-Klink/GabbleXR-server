# Gabble

Gabble is a C++ OpenXR client designed to bridge **XR_ANDROID_face_tracking** data from a WiVRn runtime to **Resonite** (or other OSC-compatible applications) using the VRCFT Unified Expressions (v2) protocols.

## Features

- **WiVRn Compatibility**: Connects to the WiVRn OpenXR runtime.
- **Face Tracking**: Implements the `XR_ANDROID_face_tracking` extension to read high-fidelity facial parameters.
- **OSC Broadcasting**: Sends tracking data to `127.0.0.1:9000` at 100Hz.
- **Protocols Supported**:
  - **VRCFT Unified Expressions (v2)**: Standardized avatar parameters mapped to Resonite properties
  - **Eye Tracking**: Includes eye gaze (X/Y) and eyelid openness mapping.
- **Headless Mode**: Uses `XR_MND_headless` to run without requiring a VR display window on the host.

## Prerequisites

- **Nix**: With Flakes and `nix-command` enabled.
- **WiVRn**: Ensure the WiVRn server is running and accessible.

## Building

The project uses a Nix flake to manage its environment and dependencies.

1.  **Enter the development shell**:
    ```bash
    nix develop
    ```

2.  **Build the project**:
    ```bash
    mkdir -p build && cd build
    cmake ..
    make
    ```

Alternatively, build in one command:
```bash
nix develop --command bash -c "mkdir -p build && cd build && cmake .. && make"
```

## Usage

1.  **Configure the OpenXR Runtime**:
    Ensure `XR_RUNTIME_JSON` points to your WiVRn runtime configuration:
    ```bash
    export XR_RUNTIME_JSON=/etc/xdg/openxr/1/active_runtime.json
    # or wherever your wivrn.json is located
    # on nix it would be export XR_RUNTIME_JSON=/home/klink/.config/openxr/1/active_runtime.json
    ```

2.  **Run Gabble**:
    ```bash
    ./build/gabble
    ```
3.  **Configure**
    The server creates config file at $HOME/.config/gabble/sensitivity.ini
    You can play around with those values

4.  **Resonite Setup**:
    Ensure Resonite is listening for OSC data on port `9000`. The tracking data will be automatically mapped if you have a compatible Babble or VRCFT-ready avatar.

