{ gcc
, libcmarkdown
, stdenv
}: stdenv.mkDerivation {
  pname = "uni-compiler";
  version = "0.1";

  buildInputs = [ gcc libcmarkdown ];
  src = ./.;

  buildPhase = ''
    gcc uni.c -lcmarkdown -o uni-compiler
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp uni-compiler $out/bin
  '';
}
