# Renderizador

- De modo geral, o renderizador é a entidade responsável por desenhar vértices na tela, juntamente com texturas, iluminação e efeitos diversos. Pode ser dividido em duas partes: back-_end_ e _front-end_.
- O _front-end_ é responsável por gerenciar diversos recursos, dentre eles:
  - _Meshes_ (conjunto de vértices)
  - Texturas
  - Materiais
  - Grafo de elementos
  - etc...
- O _back-end_ é responsável por interagir com APIs de renderização e gerenciar o envio de dados para GPU (por meio de _uploads_ e _draw calls_) . Algumas APIs são:
  - Vulkan (baixo nível, mas fornece maior controle ao usuário)
  - OpenGL (mais fácil de aprender, mas é datado)
  - DirectX (API nativa do Windows e Xbox)

Referências:

- [Vídeo](https://www.youtube.com/watch?v=70q9jtjWP4o&list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj&index=15)
