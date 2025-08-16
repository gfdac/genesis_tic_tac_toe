#ifndef GAME_H
#define GAME_H

// Enumeração para o estado de cada célula do tabuleiro
typedef enum {
    CELL_EMPTY = 0, // Célula vazia
    CELL_X = 1,     // Célula marcada com X
    CELL_O = 2      // Célula marcada com O
} CellState;

// Enumeração para o estado geral do jogo
typedef enum {
    GAME_STATE_PLAYING, // O jogo está em andamento
    GAME_STATE_WIN,     // Um jogador venceu
    GAME_STATE_DRAW     // O jogo terminou em empate
} GameStatus;

// Estrutura que contém todos os dados do estado do jogo
typedef struct {
    CellState board[9];     // Array de 9 posições representando o tabuleiro
    int currentPlayer;      // Jogador atual (1 para X, 2 para O)
    GameStatus status;      // Estado atual do jogo (jogando, vitória, empate)
    int winner;             // Armazena quem venceu (1 para X, 2 para O, 0 se ninguém)
    int scoreX;             // Placar do jogador X
    int scoreO;             // Placar do jogador O
    int draws;              // Número de empates
} TicTacToeGame;

/**
 * @brief Inicializa o estado do jogo pela primeira vez (zera placares).
 * @param game Ponteiro para a estrutura do jogo.
 */
void Game_Init(TicTacToeGame* game);

/**
 * @brief Reinicia o tabuleiro para uma nova partida, mantendo os placares.
 * @param game Ponteiro para a estrutura do jogo.
 */
void Game_Reset(TicTacToeGame* game);

/**
 * @brief Tenta realizar uma jogada em uma determinada posição do tabuleiro.
 * @param game Ponteiro para a estrutura do jogo.
 * @param position A posição no tabuleiro (0-8) onde o jogador quer jogar.
 * @return Retorna 1 se a jogada foi válida e realizada, 0 caso contrário.
 */
int Game_MakeMove(TicTacToeGame* game, int position);

#endif // GAME_H
