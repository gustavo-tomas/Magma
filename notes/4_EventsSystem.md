# Gerenciador de eventos

- Para gerenciar os eventos que ocorrem em um jogo (como inputs, redimensionamento da tela, etc) é necessário um gerenciador de eventos.
- De modo genérico, funções podem ser registradas para 'escutar' eventos específicos. Quando esses eventos ocorrem, o gerenciador de eventos chama uma função de _callback_, que decide o comportamento adequado para o evento escolhido.
  > Observação: um sistema de eventos pode ser bloqueante. Por isso, é importante que o tratamento de eventos ocorra em uma _thread_ separada para não interromper o fluxo do jogo. Além disso, eventos podem ter diferentes prioridades (fechar o jogo pode ser mais importante que redimensionar a janela, por exemplo), de forma que implementar uma lista de prioridade para armazenar e tratar os eventos mais importantes primeiro pode ser uma boa ideia.

Referências:

- Capítulo 15.7 (Game Engine Architecture - 2ª edição)

- [Vídeo](https://www.youtube.com/watch?v=XQ6qljkh2KI&list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj&index=11)
