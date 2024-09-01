{ gcc
, libuni-highlighter
, libcmarkdown
, fetchgit
, stdenv
}: stdenv.mkDerivation {
  pname = "uni-compiler";
  version = "0.1";

  buildInputs = [
    gcc
    libcmarkdown libuni-highlighter
  ];
  src = ./.;

  buildPhase = ''
    gcc uni.c \
    -lcmarkdown -luni_highlighter \
    -o uni-compiler
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp uni-compiler $out/bin
  '';
}
