{ markdown
, stdenv
}: stdenv.mkDerivation {
  name = "uni";
  src = ./content;
  buildInputs = [ markdown ];
  buildPhase = ''
    base=$(tr -d '\n' < ${./base.html})
    find "$src" -type f -name "*.md" | while read -r file; do
      rel_path="''${file#$src/}"
      dest_file="$out/''${rel_path%.md}.html"
      content=$(markdown "$file")
      mkdir -p "$(dirname "$dest_file")"
      echo "''${base/\{\}/$content}" > "$dest_file"
    done
  '';

  installPhase = ''
    mkdir -p $out/static
    cp ${./base.css} $out/static/base.css
  '';
}
