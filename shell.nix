{ gcc
, libcmarkdown
, tree-sitter
, clang-tools
, valgrind
, grass-sass
, mkShell
}: mkShell {
  buildInputs = [
    gcc
    libcmarkdown tree-sitter
    clang-tools valgrind
    grass-sass
  ];
}
