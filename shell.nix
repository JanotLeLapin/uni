{ clang-tools
, cmarkdown
, valgrind
, mkShell
}: mkShell {
  buildInputs = [ clang-tools cmarkdown valgrind ];
}
