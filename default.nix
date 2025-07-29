{ cmarkdown
, stdenv
}: stdenv.mkDerivation {
  pname = "uni";
  version = "0.1";

  buildInputs = [ cmarkdown ];
  src = ./.;

  buildPhase = ''
    $CC -Wall -Wextra -O3 -lcmarkdown main.c -o main
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp main $out/bin/uni
  '';
}
