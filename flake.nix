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
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "gabble";
        version = "0.1.0";

        src = ./.;

        nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
        ];

        buildInputs = with pkgs; [
          openxr-loader
          liblo
        ];

        meta = with pkgs.lib; {
          description = "Gabble: OpenXR face tracking client for WiVRn with OSC output";
          homepage = "https://github.com/Tony-Klink/GabbleXR-server";
          license = licenses.agpl3Only;
          platforms = platforms.linux;
        };
      };

      devShells.${system}.default = pkgs.mkShell {
        inputsFrom = [ self.packages.${system}.default ];

        shellHook = ''
          export XR_RUNTIME_JSON=/etc/xdg/openxr/1/active_runtime.json
        '';
      };

      apps.${system}.default = {
        type = "app";
        program = "${self.packages.${system}.default}/bin/gabble";
      };
    };
}
