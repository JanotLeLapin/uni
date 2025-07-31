{ uni
, callPackage
, stdenv
}: let
  uni-meta = callPackage ./meta.nix {};
in stdenv.mkDerivation {
  name = "uni-notes";
  src = ./.;
  buildInputs = [ uni uni-meta ];
  buildPhase = ''
    uni-meta
  '';
  installPhase = ''
    mkdir -p $out
    mv index.html $out
  '';
}
