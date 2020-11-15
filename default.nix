let
  pkgs = import <nixpkgs> {};
  inherit (pkgs)  lib;

  drv = { stdenv, cmake, pkg-config, libyaml, bootstrap ? false }: stdenv.mkDerivation ({
    name = "rewrite-tbd";
    src = lib.sourceFilesBySuffices ./. [".h" ".cpp" "CMakeLists.txt" "Makefile.boot"];

    nativeBuildInputs = [ pkg-config ] ++ lib.optionals (!bootstrap) [ cmake ];
    buildInputs = [ libyaml ];
  } // lib.optionalAttrs bootstrap {
    makefile = "Makefile.boot";
    makeFlags = "PREFIX=${placeholder "out"}";
  });
in
rec {
  full = pkgs.callPackage drv {};
  boot = full.override { bootstrap = true; };
}
