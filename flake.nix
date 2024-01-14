{
  description = "My note-taking repository";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    notchka.url = "github:JanotLeLapin/notchka";
  };

  outputs = {
    nixpkgs,
    notchka,
    ...
  }:
  let
    eachSystem = fn: nixpkgs.lib.genAttrs [
      "x86_64-linux"
      "aarch64-linux"
    ] (system: let
      pkgs = import nixpkgs { inherit system; };
      notchka-pkg = notchka.packages."${system}".default;
    in (fn { inherit system pkgs notchka-pkg; }));
  in {
    devShells = eachSystem ({ pkgs, notchka-pkg, ... }: {
      default = pkgs.mkShell {
        buildInputs = [ notchka-pkg ];
      };
    });
    packages = eachSystem ({ pkgs, notchka-pkg, ... }: {
      default = pkgs.stdenv.mkDerivation {
        name = "uni";
        version = "0.1.0";
        src = ./.;

        buildInputs = [ notchka-pkg ];
        buildPhase = "notchka build --prefix /uni";
        installPhase = "cp -r build $out";
      };
    });
  };
}
