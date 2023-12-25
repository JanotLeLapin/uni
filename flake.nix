{
  description = "My note-taking repository";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    notchka.url = "github:JanotLeLapin/notchka";
    systems.url = "github:nix-systems/default";
    flake-utils.url = "github:Numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    notchka,
    flake-utils,
    ...
  }: flake-utils.lib.eachDefaultSystem (system: let
    pkgs = import nixpkgs { inherit system; };
    ssg = notchka.packages.${system}.default;
  in rec {
    devShells.default = pkgs.mkShell {
      packages = [ ssg ];
    };
    packages.default = pkgs.stdenv.mkDerivation {
      name = "uni";
      version = "0.1.0";
      src = ./.;

      buildInputs = [ ssg ];
      buildPhase = "notchka build --prefix /uni";
      installPhase = "cp -r build $out";
    };
  });
}
