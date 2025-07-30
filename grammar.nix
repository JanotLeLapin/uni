{ lang
, version
, rev
, url ? "https://github.com/tree-sitter/tree-sitter-${lang}"
}:
  { gnumake
  , stdenv
  , ...
  }: stdenv.mkDerivation {
    inherit version;
    pname = "tree-sitter-${lang}";

    buildInputs = [ gnumake ];
    src = fetchGit {
      inherit url rev;
    };

    buildPhase = ''
      make
    '';
    installPhase = ''
      mkdir -p $out/lib
      cp libtree-sitter-${lang}.so $out/lib
      cp libtree-sitter-${lang}.a $out/lib
      cp queries/highlights.scm $out/lib
    '';
  }
