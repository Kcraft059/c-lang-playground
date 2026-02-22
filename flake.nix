{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs =
    { nixpkgs, ... }@inputs:
    let
      system = "aarch64-linux";

      pkgs = import nixpkgs {
        inherit system;
      };
    in
    {
      devShells."${system}".default = pkgs.mkShell {
        packages = with pkgs; [
          libgpiod # Linux GPIO lib
          lgpio # Simplified GPIO lib

          # linuxPackages.kernel.dev
          # pigpio # Pi-specific GPIO control
          gnumake
          bear
          clang
        ];

        shellHook = ''
          cat > .clangd <<EOF
          CompileFlags:
            Add:
              - -I${pkgs.glibc.dev}/include
              - -I${pkgs.lgpio}/include
          EOF

          exec ${pkgs.zsh}/bin/zsh
        '';
      };
    };
}
