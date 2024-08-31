{ gcc
, libcmarkdown
, tree-sitter
, fetchgit
, stdenv
}: let
  tree-sitter-python = fetchgit {
    url = "https://github.com/tree-sitter/tree-sitter-python";
    hash = "sha256-Mg/U7X9uvHEwo06bo0aBmJHbH9wsZ+nKBt0Dpz4o6l0=";
  };
in stdenv.mkDerivation {
  pname = "uni-compiler";
  version = "0.1";

  buildInputs = [
    gcc
    libcmarkdown tree-sitter
  ];
  src = ./.;

  buildPhase = ''
    gcc uni.c \
    -lcmarkdown -ltree-sitter \
    ${tree-sitter-python}/src/parser.c ${tree-sitter-python}/src/scanner.c \
    -o uni-compiler
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp uni-compiler $out/bin
  '';
}
