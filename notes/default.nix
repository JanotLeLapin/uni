{ uni
, stdenv
}: stdenv.mkDerivation {
  name = "uni-notes";
  src = ./.;
  buildInputs = [ uni ];
  buildPhase = ''
    find "$src" -type f -name "*.md" | while read -r file; do
      rel_path="''${file#$src/}"
      dest_file="$out/''${rel_path%.md}.html"
      content=$(uni "$file")
      mkdir -p "$(dirname "$dest_file")"
      echo "$content" > "$dest_file"
    done
  '';
}
