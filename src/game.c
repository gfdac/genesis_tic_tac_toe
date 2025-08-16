#include "inc/game.h"

// Declaração de uma função auxiliar interna (privada a este arquivo)
static void check_win_condition(TicTacToeGame* game);

void Game_Init(TicTacToeGame* game) {
    game->scoreX = 0;
    game->scoreO = 0;
    game->draws = 0;
    Game_Reset(game);
}

void Game_Reset(TicTacToeGame* game) {
    for (int i = 0; i < 9; i++) {
        game->board[i] = CELL_EMPTY;
    }
    game->currentPlayer = 1; // X sempre começa
    game->status = GAME_STATE_PLAYING;
    game->winner = 0;
}

int Game_MakeMove(TicTacToeGame* game, int position) {
    // A jogada só é válida se o jogo estiver em andamento e a célula estiver vazia
    if (game->status == GAME_STATE_PLAYING && game->board[position] == CELL_EMPTY) {
        // Define a célula com a marca do jogador atual
        game->board[position] = (game->currentPlayer == 1) ? CELL_X : CELL_O;
        
        // Verifica se a jogada resultou em vitória ou empate
        check_win_condition(game);
        
        // Se o jogo ainda estiver em andamento após a verificação, troca o jogador
        if (game->status == GAME_STATE_PLAYING) {
            game->currentPlayer = (game->currentPlayer == 1) ? 2 : 1;
        }
        
        return 1; // Jogada bem-sucedida
    }
    
    return 0; // Jogada inválida
}

// Função auxiliar que verifica o estado do jogo (vitória, empate ou continua)
static void check_win_condition(TicTacToeGame* game) {
    // Checa linhas
    for (int i = 0; i < 3; i++) {
        if (game->board[i*3] != CELL_EMPTY && game->board[i*3] == game->board[i*3+1] && game->board[i*3+1] == game->board[i*3+2]) {
            game->status = GAME_STATE_WIN;
            game->winner = game->board[i*3];
            if (game->winner == CELL_X) game->scoreX++; else game->scoreO++;
            return;
        }
    }

    // Checa colunas
    for (int i = 0; i < 3; i++) {
        if (game->board[i] != CELL_EMPTY && game->board[i] == game->board[i+3] && game->board[i+3] == game->board[i+6]) {
            game->status = GAME_STATE_WIN;
            game->winner = game->board[i];
            if (game->winner == CELL_X) game->scoreX++; else game->scoreO++;
            return;
        }
    }

    // Checa diagonais
    if (game->board[0] != CELL_EMPTY && game->board[0] == game->board[4] && game->board[4] == game->board[8]) {
        game->status = GAME_STATE_WIN;
        game->winner = game->board[0];
        if (game->winner == CELL_X) game->scoreX++; else game->scoreO++;
        return;
    }
    if (game->board[2] != CELL_EMPTY && game->board[2] == game->board[4] && game->board[4] == game->board[6]) {
        game->status = GAME_STATE_WIN;
        game->winner = game->board[2];
        if (game->winner == CELL_X) game->scoreX++; else game->scoreO++;
        return;
    }

    // Checa empate (se não houve vencedor e não há mais células vazias)
    for (int i = 0; i < 9; i++) {
        if (game->board[i] == CELL_EMPTY) {
            return; // Jogo continua, pois encontrou uma célula vazia
        }
    }

    // Se o loop terminar, não há mais células vazias e ninguém venceu, então é empate
    game->status = GAME_STATE_DRAW;
    game->draws++;
}
