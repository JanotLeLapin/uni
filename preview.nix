{ uni-content
, static-web-server
, writeScriptBin
}: writeScriptBin "uni-preview" ''
  ${static-web-server}/bin/static-web-server --port 8080 --root ${uni-content}
''
