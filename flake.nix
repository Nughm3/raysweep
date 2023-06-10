{
  description = "Minesweeper with raylib";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        raysweep = pkgs.stdenv.mkDerivation {
          pname = "raysweep";
          version = "0.1.0";
          src = ./.;

          buildInputs = with pkgs; [
            raylib
          ];

          nativeBuildInputs = with pkgs; [
            meson
            ninja
            pkg-config
          ];
        };
      in
      {
        packages.default = raysweep;

        apps.default = flake-utils.lib.mkApp {
          drv = raysweep;
        };

        devShells.default = pkgs.mkShell {
          buildInputs = raysweep.buildInputs;
          nativeBuildInputs = raysweep.nativeBuildInputs;
        };
      }
    );
}
