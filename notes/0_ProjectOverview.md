# Estrutura do projeto

- A engine consiste em uma DLL, um editor e a aplicação em si (_.exe_).
- Algumas camadas de abstração:
  - Camada de plataforma (I/O, console, etc)
  - Camada de aplicação (loop do jogo)
  - API do renderizador

# Arquitetura da engine

- Camada da plataforma (APIs do Windows/Linux, memória, arquivos)
- Núcleo (asserções, estruturas de dados, _profiling_, biblioteca de matemática)
- Gerenciamento de recursos (imagens, materiais, animações)
- Renderizador (_front-end_)
  - GUI
  - Câmera
  - Pós-processamento
- Renderizador (_back-end_)
  - Abstração da API
  - Shaders
  - Texturas
  - Materiais
- Áudio

etc...

Referências:

- Capítulo 1.6 (Game Engine Architecture - 2ª edição)
- [Vídeo](https://www.youtube.com/watch?v=teg23SJlyl8&list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj&index=2)
