# Jogo da Velha Multi-Plataforma

Este projeto é uma implementação do clássico Jogo da Velha com uma arquitetura de software projetada para máxima portabilidade entre diferentes plataformas de hardware, começando com o Mega Drive.

O objetivo principal é separar completamente o **núcleo de lógica do jogo (Core)** da **camada de plataforma (Platform Layer)**, permitindo que o mesmo núcleo seja reutilizado em consoles como PSP, Nintendo Switch, Master System, Game Boy, e outros, com o mínimo de esforço.

## Arquitetura

A arquitetura do software é dividida em duas camadas distintas para garantir a separação de responsabilidades e facilitar a portabilidade.

```
+--------------------------------------+
|      Camada de Plataforma            |  <-- Específico do Hardware (Ex: Mega Drive)
| (main.c, platform_drawing.c, etc.)   |
|--------------------------------------|
| - Leitura de Controles (Joystick)    |
| - Renderização de Gráficos e UI      |
| - Gerenciamento de Som               |
| - Loop Principal e Timings           |
+--------------------------------------+
                |
                | (Inclui e chama a API do Core)
                V
+--------------------------------------+
|      Núcleo de Lógica do Jogo (Core) |  <-- Genérico e Reutilizável
| (game.c, game.h)                     |
|--------------------------------------|
| - Estado do Tabuleiro                |
| - Regras de Vitória/Empate           |
| - Gerenciamento de Turnos e Placar   |
| - API Pública (ex: Game_Init, etc.)  |
+--------------------------------------+
```

### Vantagens desta Arquitetura

*   **Reutilização de Código**: Mais de 90% do código (o núcleo do jogo) pode ser reutilizado em qualquer plataforma.
*   **Manutenção Simplificada**: Correções de bugs ou novas funcionalidades na lógica do jogo são feitas uma única vez no Core e funcionam para todas as plataformas.
*   **Portabilidade Rápida**: Para portar o jogo para um novo console, basta implementar a camada de plataforma (entrada, vídeo, som), sem tocar na lógica do jogo.

## Estrutura de Arquivos

```
/
├── inc/                # Arquivos de cabeçalho (.h)
│   └── game.h          # API pública do núcleo do jogo (a ser criada)
├── src/                # Código fonte (.c)
│   ├── boot/           # Código de inicialização do SGDK
│   ├── main.c          # Camada de plataforma (específico do Mega Drive)
│   └── game.c          # Implementação do núcleo do jogo (a ser criada)
├── res/                # Recursos (gráficos, som, etc.)
├── out/                # Arquivos de saída da compilação (ROMs)
├── .gitignore
├── Makefile            # Script de compilação para ambientes Unix/Linux
└── README.md           # Esta documentação
```

## Como Compilar (Mega Drive)

Este projeto utiliza o [SGDK (Sega Genesis Development Kit)](https://github.com/Stephane-D/SGDK). Certifique-se de ter o ambiente SGDK configurado.

1.  **Clone o repositório:**
    ```sh
    git clone <url-do-repositorio>
    ```
2.  **Navegue até o diretório do projeto:**
    ```sh
    cd genesis_test
    ```
3.  **Compile o projeto:**
    *   No Windows, você pode usar o `build.bat`.
    *   No Linux/macOS, use o `Makefile`:
        ```sh
        make -f Makefile
        ```
4.  **Execute a ROM:**
    A ROM compilada (`rom.bin`) estará no diretório `out/`. Use um emulador de Mega Drive (como o BlastEm ou Genesis Plus GX) para executá-la.

## Guia para Portar para Outras Plataformas

Para adaptar este projeto para uma nova plataforma (ex: PSP), siga estes passos:

1.  **Crie um novo diretório de projeto** para a plataforma alvo.
2.  **Copie os arquivos do Core**: `inc/game.h` e `src/game.c` para o novo projeto.
3.  **Implemente a Camada de Plataforma**: Crie um novo `main.c` que:
    *   Use o SDK específico da nova plataforma (ex: PSP SDK).
    *   Implemente a leitura de controles.
    *   Implemente as funções de renderização para desenhar o tabuleiro e os elementos do jogo.
    *   Chame as funções da API do Core (`Game_Init()`, `Game_MakeMove()`, etc.) para gerenciar o estado do jogo.
    *   Renderize o estado do jogo retornado pelo Core na tela.

## Próximos Passos

O próximo passo neste projeto é refatorar o código existente em `src/main.c` para separar a lógica do jogo nos novos arquivos `game.c` e `game.h`, seguindo a arquitetura descrita acima.
