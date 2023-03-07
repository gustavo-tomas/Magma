# Camada da aplicação

- A camada de aplicação deve abstrair a construção da aplicação do resto da engine (a engine não precisa saber se a janela foi criada em linux ou windows).
- A aplicação é encarregada de inicializar os subsistemas (como memória, _logger_, etc) e executar o _game loop_.

# Ponto de entrada

- O ponto de entrada da aplicação contém a função _main_. A declaração do jogo é feita pela engine, mas o funcionamento (e criação do jogo) deve ser ditado pelo usuário.

Referências:

- [Vídeo](https://www.youtube.com/watch?v=jEbqCf-MsZs&list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj&index=9)
