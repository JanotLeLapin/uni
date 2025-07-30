{ clang-tools
, cmarkdown
, tree-sitter
, valgrind
, xxd
, mkShell
}: mkShell {
  buildInputs = [ clang-tools cmarkdown tree-sitter valgrind xxd ];
}
