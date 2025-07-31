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
          tree-sitter-java = import ./grammar.nix {
            lang = "java";
            version = "0.23.5";
            rev = "94703d5a6bed02b98e438d7cad1136c01a60ba2c";
          } self.pkgsMusl;
          tree-sitter-c = import ./grammar.nix {
            lang = "c";
            version = "0.24.1";
            rev = "7fa1be1b694b6e763686793d97da01f36a0e5c12";
          } self.pkgsMusl;
          tree-sitter-bash = import ./grammar.nix {
            lang = "bash";
            version = "0.25.0";
            rev = "56b54c61fb48bce0c63e3dfa2240b5d274384763";
          } self.pkgsMusl;
          tree-sitter-nix = (import ./grammar.nix {
            lang = "nix";
            version = "0.3.0";
            rev = "ea1d87f7996be1329ef6555dcacfa63a69bd55c6";
            url = "https://github.com/nix-community/tree-sitter-nix";
          } self.pkgsMusl).overrideAttrs (oldAttrs: {
            buildPhase = ''
              $CC -Isrc -std=c11 -fPIC -c -o parser.o src/parser.c
              $CC -Isrc -std=c11 -fPIC -c -o scanner.o src/scanner.c
              $CC -shared -Wl,-soname,libtree-sitter-nix.so.13.0 parser.o scanner.o -o libtree-sitter-nix.so
              ar -rv libtree-sitter-nix.a parser.o scanner.o
            '';
          });
        })];
      });
    }));
  in {
    devShells = eachSystem ({ pkgs, ... }: { default = pkgs.callPackage ./shell.nix {}; });
    packages = eachSystem ({ pkgs, ... }: {
      default = pkgs.pkgsMusl.callPackage ./default.nix {};
      minimal = pkgs.pkgsMusl.callPackage ./default.nix {
        enableTreeSitter = false;
        enableTableOfContents = false;
      };
    });
  };
}
