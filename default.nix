let
  pkgs = import <nixpkgs> {};
  inherit (pkgs)  lib;

  drv = { stdenv, cmake, pkg-config, libyaml, bootstrap ? false }: stdenv.mkDerivation {
    name = "rewrite-tbd";
    src = lib.sourceFilesBySuffices ./. [".h" ".cpp" "CMakeLists.txt" "Makefile.boot"];

    nativeBuildInputs = [ pkg-config ] ++ lib.optionals (!bootstrap) [ cmake ];
    buildInputs = [ libyaml ];

    makefile = if bootstrap then "Makefile.boot" else null;

    installPhase = ''
      mkdir -p $out/bin
      cp rewrite-tbd $out/bin
    '';
  };
in
rec {
  full = pkgs.callPackage drv {};
  boot = full.override { bootstrap = true; };
}
