{ mkShell
, nodejs
}: mkShell {
  buildInputs = [ nodejs ];
}
