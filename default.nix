{ markdown
, stdenv
}: stdenv.mkDerivation {
  name = "uni";
  src = ./content;
  buildInputs = [ markdown ];
  buildPhase = ''
    find "$src" -type f -name "*.md" | while read -r file; do
      rel_path="''${file#$src/}"
      dest_file="$out/''${rel_path%.md}.html"
      mkdir -p "$(dirname "$dest_file")"
      markdown "$file" > "$dest_file"
    done
  '';
}
