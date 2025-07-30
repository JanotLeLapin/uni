{ clang-tools
, cmarkdown
, tree-sitter
, valgrind
, xxd
, mkShell
}: mkShell {
  buildInputs = [ clang-tools cmarkdown tree-sitter valgrind xxd ];
  shellHook = ''
    mkdir -p style
    mkdir -p highlights
    xxd -i -n style_code ./code.css style/code.h
    xxd -i -n style_app ./app.css style/app.h
  '';
}
