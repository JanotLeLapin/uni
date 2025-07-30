{ xxd
, cmarkdown
, tree-sitter
, tree-sitter-json
, tree-sitter-python

, enableTreeSitter ? true
, enableJsonGrammar ? true
, enablePythonGrammar ? true
, stdenv
}: let
  featureFlags = if enableTreeSitter then [
    "-DENABLE_TREE_SITTER"
    (if enableJsonGrammar != null then "-DENABLE_JSON_GRAMMAR" else "")
    (if enablePythonGrammar != null then "-DENABLE_PYTHON_GRAMMAR" else "")
  ] else [];

  libraryFlags = if enableTreeSitter then ([ "-lcmarkdown" "-ltree-sitter" ] ++
    (if enableJsonGrammar != null then [ "-ltree-sitter-json" ] else []) ++
    (if enablePythonGrammar != null then [ "-ltree-sitter-python" ] else [])
  ) else [ "-lcmarkdown" ];

  includeFlags = if enableTreeSitter then ([ "-I${cmarkdown}/include" "-I${tree-sitter}/include" ] ++
    (if enableJsonGrammar != null then [ "-I${tree-sitter-json}/include" ] else []) ++
    (if enablePythonGrammar != null then [ "-I${tree-sitter-python}/include" ] else [])
  ) else [];

  linkerFlags = if enableTreeSitter then ([ "-L${cmarkdown}/lib" "-L${tree-sitter}/lib" ] ++
    (if enableJsonGrammar != null then [ "-L${tree-sitter-json}/lib" ] else []) ++
    (if enablePythonGrammar != null then [ "-L${tree-sitter-python}/lib" ] else [])
  ) else [];

  sources = if enableTreeSitter then ([ "main.c" "highlight.c" ]) else ([ "main.c" ]);
in stdenv.mkDerivation {
  pname = "uni";
  version = "0.1";

  nativeBuildInputs = [ xxd ];
  buildInputs = [ cmarkdown tree-sitter ];
  src = ./.;

  buildPhase = ''
    mkdir -p style
    mkdir -p highlights
    ${if enableTreeSitter then ''
      xxd -i -n style_code ./code.css style/code.h
      ${if enableJsonGrammar then "xxd -i -n highlights_json ${tree-sitter-json}/lib/highlights.scm highlights/json.h" else ""}
      ${if enablePythonGrammar then "xxd -i -n highlights_python ${tree-sitter-python}/lib/highlights.scm highlights/python.h" else ""}
      xxd -i -n highlights_python ${tree-sitter-python}/lib/highlights.scm highlights/python.h
    '' else ""}
    $CC -static -Wall -Wextra -O3 \
      ${builtins.concatStringsSep " " (builtins.filter (x: x != "") featureFlags)} \
      ${builtins.concatStringsSep " " includeFlags} \
      ${builtins.concatStringsSep " " linkerFlags} \
      ${builtins.concatStringsSep " " sources} \
      ${builtins.concatStringsSep " " libraryFlags} \
      -o main
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp main $out/bin/uni
  '';
}
