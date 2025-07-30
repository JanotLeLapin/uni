{ clang-tools
, cmarkdown
, tree-sitter
, valgrind
, xxd
, tree-sitter-json
, tree-sitter-python
, mkShell
}: mkShell {
  buildInputs = [ clang-tools cmarkdown tree-sitter valgrind xxd ];
  shellHook = ''
    mkdir -p style
    mkdir -p highlights
    xxd -i -n style_code ./code.css style/code.h
    xxd -i -n highlights_json ${tree-sitter-json}/lib/highlights.scm highlights/json.h
    xxd -i -n highlights_python ${tree-sitter-python}/lib/highlights.scm highlights/python.h
  '';
}
