{ gcc
, libcmarkdown
, tree-sitter
, clang-tools
, valgrind
, mkShell
}: mkShell {
  buildInputs = [
    gcc
    libcmarkdown tree-sitter
    clang-tools valgrind
  ];
}
