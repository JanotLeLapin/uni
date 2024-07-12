{ vscode-langservers-extracted
, mkShell
}: mkShell {
  buildInputs = [ vscode-langservers-extracted ];
}
