{ clang-tools
, cmarkdown
, tree-sitter
, valgrind
, mkShell
}: mkShell {
  buildInputs = [ clang-tools cmarkdown tree-sitter valgrind ];
}
