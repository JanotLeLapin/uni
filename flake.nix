{
  description = "My note-taking repository";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    systems.url = "github:nix-systems/default";
    notchka.url = "github:JanotLeLapin/notchka";
  };

  outputs = {
    self,
    nixpkgs,
    systems,
    notchka,
    ...
  }:
  let
    eachSystem = nixpkgs.lib.genAttrs (import systems);
    pkgs = system: import nixpkgs { inherit system; };
  in {
    devShells = eachSystem (system: {
      default = (pkgs system).mkShell {
        packages = [ (notchka.package { inherit system; dev = true; }) ];
      };
    });
    packages = eachSystem (system: {
      default = (pkgs system).stdenv.mkDerivation {
        name = "uni";
        version = "0.1.0";
        src = ./.;

        buildInputs = [ (notchka.package { inherit system; dev = false; }) ];
        buildPhase = "notchka build --prefix /uni";
        installPhase = "cp -r build $out";
      };
    });
  };
}
