{ cmarkdown
, tree-sitter
, tree-sitter-json
, tree-sitter-python
, stdenv
}: stdenv.mkDerivation {
  pname = "uni";
  version = "0.1";

  buildInputs = [ cmarkdown tree-sitter ];
  src = ./.;

  buildPhase = ''
    $CC -static -Wall -Wextra -O3 \
      -I${cmarkdown}/include -L${cmarkdown}/lib \
      -I${tree-sitter}/include -L${tree-sitter}/lib \
      -I${tree-sitter-json}/include -L${tree-sitter-json}/lib \
      -I${tree-sitter-python}/include -L${tree-sitter-python}/lib \
      main.c highlight.c \
      -lcmarkdown -ltree-sitter \
      -ltree-sitter-json \
      -ltree-sitter-python \
      -o main
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp main $out/bin/uni
  '';
}
