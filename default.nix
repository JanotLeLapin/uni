{ cmarkdown
, tree-sitter
, tree-sitter-json
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
      main.c -lcmarkdown -ltree-sitter -ltree-sitter-json -o main
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp main $out/bin/uni
  '';
}
