{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
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

            configurePhase = ''
              substituteInPlace configure.h \
                --replace "/usr/share/games/xdigger" \
                          "$out/share/games/xdigger"
              substituteInPlace Imakefile \
                --replace "/usr/games" \
                          "$out/bin" \
                --replace "/usr/share/man/man6" \
                          "$out/share/man/man6" \
                --replace "/usr/share/pixmaps" \
                          "$out/share/pixmaps"
              substituteInPlace xdigger.desktop \
                --replace "Exec=xdigger" \
                          "Exec=$out/xdigger"
              xmkmf
            '';

            buildPhase = ''
              make
            '';

            installPhase = ''
              make install
              make install.man

              mkdir -p $out/share/applications/
              install xdigger.desktop $out/share/applications/
            '';

            nativeBuildInputs = with pkgs; [
              xorg.imake
              xorg.gccmakedep
            ];

            buildInputs = with pkgs; [
              xorg.libX11
              xorg.libXext
            ];
          };
        };
      }
    );
}
