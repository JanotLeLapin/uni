{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs";
  inputs.cmarkdown.url = "github:JanotLeLapin/cmarkdown";
  inputs.rust-overlay.url = "github:oxalica/rust-overlay";

  outputs = {
    nixpkgs,
    cmarkdown,
    rust-overlay,
    ... 
  }: let
    eachSystem = fn: nixpkgs.lib.genAttrs [
      "x86_64-linux"
      "aarch64-linux"
    ] (system: (fn {
      inherit system;
      pkgs = (import nixpkgs {
        inherit system;
        overlays = [ (import rust-overlay) ];
      });
    }));
    libcmarkdown = system: cmarkdown.packages."${system}".default;
  in {
    devShells = eachSystem ({ pkgs, system, ... }: { default = pkgs.callPackage ./shell.nix { libcmarkdown = cmarkdown.packages."${system}".default; }; });
    packages = eachSystem ({ pkgs, system, ... }: rec {
      libuni-highlighter = pkgs.callPackage ./compiler/highlighter/default.nix {};
      uni-compiler = pkgs.callPackage ./compiler/default.nix { libcmarkdown = (libcmarkdown system); inherit libuni-highlighter; };
      default = pkgs.callPackage ./notes/default.nix { inherit uni-compiler; };
    });
  };
}
