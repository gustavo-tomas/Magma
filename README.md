# Magma

Uma engine poderosa, capaz de assustar um camponês do século XIV. Projeto pessoal para aprender o funcionamento de um motor de jogos 3D. Feito em _vanilla_ C e [Vulkan](https://www.vulkan.org/).

> **Por enquanto, só suporta linux!**

## Gerando Makefiles

Para gerar os makefiles (da _engine_ e do jogo) execute o [premake](https://premake.github.io/):

```
./scripts/premake5 gmake
```

## Compilando a engine e o jogo

Depois que os arquivos Makefile foram gerados, execute o seguinte comando:

```
make
```

## Rodando o jogo

Por fim, rode o binário gerado em `bin/sandbox/sandbox`:

```
bin/sandbox/sandbox
```

## Notas

A pasta [notes](notes) contém notas sobre o funcionamento de alguns sistemas da engine que achei relevante anotar. As notas são apenas meu entendimento sobre o assunto, não é nada muito acadêmico.

## Referências

Aqui estão os recursos que mais me ajudaram a entender o processo:

- [Vulkan Game Engine Series - Travis Vroman (Playlist)](https://www.youtube.com/playlist?list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj)
- [Documentação do Vulkan](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html)
- [Book of Shaders](https://thebookofshaders.com/)
- [Game Engine - TheCherno (Playlist)](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)
