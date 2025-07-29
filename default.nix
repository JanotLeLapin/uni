{ cmarkdown
, tree-sitter
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
      main.c -lcmarkdown -ltree-sitter -o main
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp main $out/bin/uni
  '';
}
