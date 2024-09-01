{ uni-compiler
, grass-sass
, stdenv
}: stdenv.mkDerivation {
  name = "uni-notes";
  src = ./.;
  buildInputs = [ uni-compiler grass-sass ];
  buildPhase = ''
    find "$src" -type f -name "*.md" | while read -r file; do
      rel_path="''${file#$src/}"
      dest_file="$out/''${rel_path%.md}.html"
      content=$(uni-compiler < "$file")
      mkdir -p "$(dirname "$dest_file")"
      echo "$content" > "$dest_file"
    done

    mkdir -p $out/static
    grass app.scss $out/static/app.css
  '';
}
