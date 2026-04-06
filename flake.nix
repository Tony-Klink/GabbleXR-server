{
  description = "Gabble: OpenXR face tracking client for WiVRn with OSC output";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          cmake
          pkg-config
          openxr-loader
          liblo
          gcc
          gnumake
        ];

        shellHook = ''
          export XR_RUNTIME_JSON=/etc/xdg/openxr/1/active_runtime.json
        '';
      };
    };
}
