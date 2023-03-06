# Camada da plataforma

- Uma plataforma pode ser definida como um SO ou um tipo de dispositivo (Windows, Linux, Android, ...)
- O objetivo é abstrair (separar) funcionalidades específicas de cada plataforma do resto da engine.
- Para isso, podemos usar uma interface para gerenciar as funções de cada plataforma.

# Esquema em C

- A interface, _interface.h_ é implementada pelas diversas plataformas: _platform_win32.c_, _platform_linux.c_, _platform_mac.c_, etc. Esses arquivos contém implementações específicas de cada plataforma e são usados (ou não) por meio de `#defines`.

Referências:

- [Vídeo](https://www.youtube.com/watch?v=ojEPsZFPj2Q&list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj&index=7)
