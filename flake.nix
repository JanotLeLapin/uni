{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs";
  inputs.cmarkdown.url = "github:JanotLeLapin/cmarkdown/new";

  outputs = { nixpkgs, cmarkdown, ... }: let
    eachSystem = fn: nixpkgs.lib.genAttrs [
      "x86_64-linux"
      "aarch64-linux"
    ] (system: (fn {
      inherit system;
      pkgs = (import nixpkgs {
        inherit system;
        overlays = [(self: super: {
          cmarkdown = cmarkdown.packages."${system}".musl;
          tree-sitter = self.pkgsMusl.callPackage ./tree-sitter.nix {};
          tree-sitter-json = import ./grammar.nix {
            lang = "json";
            version = "0.24.8";
            rev = "ee35a6ebefcef0c5c416c0d1ccec7370cfca5a24";
          } self.pkgsMusl;
          tree-sitter-python = import ./grammar.nix {
            lang = "python";
            version = "0.23.6";
            rev = "bffb65a8cfe4e46290331dfef0dbf0ef3679de11";
          } self.pkgsMusl;
        })];
      });
    }));
  in {
    devShells = eachSystem ({ pkgs, ... }: { default = pkgs.callPackage ./shell.nix {}; });
    packages = eachSystem ({ pkgs, ... }: { default = pkgs.pkgsMusl.callPackage ./default.nix {}; });
  };
}
