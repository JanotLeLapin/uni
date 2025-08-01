{ cmarkdown
, stdenv
}: stdenv.mkDerivation {
  pname = "uni-meta";
  version = "0.1";

  src = ./.;

  buildInputs = [ cmarkdown ];
  buildPhase = ''
    $CC -static -Wall -Wextra -O3 main.c -lcmarkdown -o main
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp main $out/bin/uni-meta
  '';
}
