{ gcc
, rust-bin
, libcmarkdown
, tree-sitter
, clang-tools
, rust-analyzer
, valgrind
, grass-sass
, mkShell
}: mkShell {
  buildInputs = [
    gcc (rust-bin.fromRustupToolchainFile ./rust-toolchain.toml)
    libcmarkdown tree-sitter
    clang-tools rust-analyzer valgrind
    grass-sass
  ];
}
