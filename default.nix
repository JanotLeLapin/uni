{ xxd
, cmarkdown
, tree-sitter
, tree-sitter-json
, tree-sitter-python
, tree-sitter-c
, tree-sitter-bash
, tree-sitter-nix
, tree-sitter-java
, lib
, stdenv

, patches ? [ ]
, enableTreeSitter ? true
, enableJsonGrammar ? true
, enablePythonGrammar ? true
, enableJavaGrammar ? true
, enableCGrammar ? true
, enableBashGrammar ? true
, enableNixGrammar ? true
, enableTableOfContents ? true
}: let
  grammars = [
    { name = "json"; package = tree-sitter-json; enabled = enableJsonGrammar; }
    { name = "python"; package = tree-sitter-python; enabled = enablePythonGrammar; }
    { name = "java"; package = tree-sitter-java; enabled = enableJavaGrammar; }
    { name = "c"; package = tree-sitter-c; enabled = enableCGrammar; }
    { name = "bash"; package = tree-sitter-bash; enabled = enableBashGrammar; }
    { name = "nix"; package = tree-sitter-nix; enabled = enableNixGrammar; }
  ];

  enabledGrammars = (builtins.filter (g: g.enabled) grammars);

  xxdCommands =
    [ "xxd -i -n style_app ./app.css style/app.h" ] ++
    (if enableTreeSitter then (
      [ "xxd -i -n style_code ./code.css style/code.h "] ++
      (map (g: "xxd -i -n highlights_${g.name} ${g.package}/lib/highlights.scm highlights/${g.name}.h") enabledGrammars)
    ) else []) ++
    (if enableTableOfContents
      then [ "xxd -i -n style_toc ./toc.css style/toc.h" ]
      else []);

  featureFlags =
    (if enableTreeSitter then (
      [ "-DENABLE_TREE_SITTER" ] ++
      (map (g: "-DENABLE_${lib.toUpper g.name}_GRAMMAR") enabledGrammars)
    ) else []) ++
    (if enableTableOfContents then [ "-DENABLE_TOC" ] else []);

  libraryFlags =
    [ "-lcmarkdown" ] ++
    (if enableTreeSitter then (
      [ "-ltree-sitter" ] ++
      (map (g: "-ltree-sitter-${g.name}") enabledGrammars)
    ) else []);

  includeFlags =
    [ "-I${cmarkdown}/include" ] ++
    (if enableTreeSitter then (
      [ "-I${cmarkdown}/include" "-I${tree-sitter}/include" ] ++
      (map (g: "-I${g.package}/include") enabledGrammars))
    else []);

  linkerFlags =
    [ "-L${cmarkdown}/lib" ] ++
    (if enableTreeSitter then (
      [ "-L${cmarkdown}/lib" "-L${tree-sitter}/lib" ] ++
      (map (g: "-L${g.package}/lib") enabledGrammars)
    ) else []);

  sources =
    [ "main.c" ] ++
    (if enableTreeSitter
      then [ "highlight.c" ]
      else []);
in stdenv.mkDerivation {
  pname = "uni";
  version = "0.1";

  nativeBuildInputs = [ xxd ];
  buildInputs = [ cmarkdown tree-sitter ];
  src = ./.;

  inherit patches;

  buildPhase = ''
    mkdir -p style
    mkdir -p highlights
    ${builtins.concatStringsSep "\n" xxdCommands}

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
