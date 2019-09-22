{ pkgs ? import <nixpkgs> { }, ... }:

pkgs.stdenvNoCC.mkDerivation rec {
  name = "wandler-env";
  env = pkgs.buildEnv { name = name; paths = buildInputs; };

  buildInputs = with pkgs; [
    cmake
    universal-ctags
    gcc9
    gdb cgdb
    glfw3
    glew
    glm
    freeglut
  ];

  shellHook = ''
    export NIX_SHELL_NAME="${name}"
  '';
}
