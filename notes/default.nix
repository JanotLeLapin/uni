{ uni
, uni-meta
, stdenv
}: stdenv.mkDerivation {
  name = "uni-notes";
  src = ./.;
  buildInputs = [ uni uni-meta ];
  buildPhase = ''
    mkdir -p out
    uni-meta
  '';
  installPhase = ''
    cp -r out $out
  '';
}
