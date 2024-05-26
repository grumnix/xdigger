{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = rec {
          default = xdigger;

          xdigger = pkgs.stdenv.mkDerivation rec {
            pname = "xdigger";
            version = "1.0.10";

            src = ./.;

            preConfigure = ''
              substituteInPlace configure.h \
                --replace "/usr/share/games/xdigger" \
                          "$out/share/games/xdigger"
              substituteInPlace xdigger.desktop \
                --replace "Exec=xdigger" \
                          "Exec=$out/bin/xdigger"
            '';

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
            ];

            buildInputs = with pkgs; [
              xorg.libX11
              xorg.libXext
              SDL2
              SDL2_mixer
            ];
          };
        };
      }
    );
}
