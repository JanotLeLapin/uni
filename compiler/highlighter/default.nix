{ rustPlatform
}: rustPlatform.buildRustPackage {
  pname = "uni-highlighter";
  version = "0.1.0";
  src = ./.;
  cargoLock = { lockFile = ./Cargo.lock; };
  nativeBuildInputs = [];
  buildInputs = [];
}
