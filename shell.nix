{ gcc
, libcmarkdown
, clang-tools
, valgrind
, mkShell
}: mkShell {
  buildInputs = [ gcc libcmarkdown clang-tools valgrind ];
}
