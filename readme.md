# uni

my note taking repository

- markdown compiler licensed under GNU GPL v3
- markdown content licensed under CC BY 4.0 (coming soon)

## get the compiler

you may use the compiler in your own projects, but do
note that it doesn't implement many features right now

```nix
# flake.nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    # add the uni flake
    uni.url = "github:JanotLeLapin/uni";
  };

  outputs = { self, nixpkgs, uni }: let
    system = "x86_64-linux";
    pkgs = (import nixpkgs {
      inherit system;
      overlays = [(self: super: {
        # add uni package
        uni = uni.packages.${system}.minimal.override {
          enableTreeSitter = true;
          enableJsonGrammar = true;
          enablePythonGrammar = false;
        };
      })];
    });
  in {
    packages.${system}.default = pkgs.callPackage ./some/package.nix {};
  };
}
```

take a look at [`default.nix`](./default.nix) to see all
the features you may override

## usage

uni compiler usage

```sh
uni [options] arguments

-t, --title   html page title
```
