{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    flake-utils.url = "github:numtide/flake-utils";

    xdigger_src.url = "https://ftp.lip6.fr/pub/linux/sunsite/games/arcade/xdigger-1.0.10.tgz";
    xdigger_src.flake = false;
  };

  outputs = { self, nixpkgs, flake-utils, xdigger_src }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = rec {
          default = xdigger;

          xdigger = pkgs.stdenv.mkDerivation rec {
            pname = "xdigger";
            version = "1.0.10";

            src = xdigger_src;

            patches = [
              ./patches/buffers.txt
              ./patches/dont-create-highscore.txt
	            ./patches/start-level-on-move.txt
              ./patches/config.txt
              ./patches/escape-hyphen-in-manpage.txt
              ./patches/open-mode.txt
            ];

            buildPhase = ''
            '';

            installPhase = ''
            '';

            nativeBuildInputs = with pkgs; [
              xorg.imake
              xorg.gccmakedep
            ];

            buildInputs = with pkgs; [
              xorg.libX11
            ];
          };
        };
      }
    );
}
