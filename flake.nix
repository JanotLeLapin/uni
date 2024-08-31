{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs";
  inputs.cmarkdown.url = "github:JanotLeLapin/cmarkdown";

  outputs = {
    nixpkgs,
    cmarkdown,
    ... 
  }: let
    eachSystem = fn: nixpkgs.lib.genAttrs [
      "x86_64-linux"
      "aarch64-linux"
    ] (system: (fn {
      inherit system;
      pkgs = (import nixpkgs { inherit system; } );
    }));
    libcmarkdown = system: cmarkdown.packages."${system}".default;
    uni-compiler = { pkgs, system }: pkgs.callPackage ./compiler/default.nix { libcmarkdown = (libcmarkdown system); };
  in {
    devShells = eachSystem ({ pkgs, system, ... }: { default = pkgs.callPackage ./shell.nix { libcmarkdown = cmarkdown.packages."${system}".default; }; });
    packages = eachSystem ({ pkgs, system, ... }: { default = pkgs.callPackage ./notes/default.nix { uni-compiler = (uni-compiler { inherit pkgs system; }); }; });
  };
}
