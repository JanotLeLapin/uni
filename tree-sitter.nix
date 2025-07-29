{ gnumake
, stdenv
}: stdenv.mkDerivation {
  pname = "tree-sitter";
  version = "0.26";

  buildInputs = [ gnumake ];
  src = fetchGit {
    url = "https://github.com/tree-sitter/tree-sitter";
    rev = "cd2cb661a41960a48211fa103f69185cc1f94ea3";
  };

  buildPhase = ''
    make
  '';
  installPhase = ''
    mkdir -p $out/lib
    cp -r lib/include $out/include
    cp libtree-sitter.so $out/lib
    cp libtree-sitter.a $out/lib
  '';
}
