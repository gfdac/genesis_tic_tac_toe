
/**
 * Jogo da Velha para Mega Drive - Camada de Plataforma
 * 
 * Este arquivo é responsável por toda a interação com o hardware do Mega Drive,
 * incluindo inicialização, renderização de gráficos e leitura de controles.
 * Ele utiliza o núcleo de jogo (game.c) para toda a lógica e regras,
 * agindo como a camada de visualização e controle.
 */

#include <genesis.h>
#include "inc/game.h"

// --- RECURSOS GRÁFICOS E DEFINIÇÕES DA PLATAFORMA ---

// Tiles para X, O e cursor
const u32 x_tile[8] = {0x01100110, 0x00111100, 0x00011000, 0x00111100, 0x01100110, 0x11000011, 0x10000001, 0x00000000};
const u32 o_tile[8] = {0x00111100, 0x01111110, 0x11000011, 0x11000011, 0x11000011, 0x01111110, 0x00111100, 0x00000000};
const u32 cursor_tile[8] = {0x33333333, 0x33000033, 0x33000033, 0x33000033, 0x33000033, 0x33000033, 0x33333333, 0x00000000};

// Paleta de cores
const u16 palette[16] = {0x0000, 0x0EEE, 0x0800, 0x0EEA, 0x00E0, 0x000E, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

// Índices dos tiles na VRAM
#define TILE_BASE_INDEX 256
#define TILE_X (TILE_BASE_INDEX)
#define TILE_O (TILE_BASE_INDEX + 1)
#define TILE_CURSOR (TILE_BASE_INDEX + 2)

// Posições X e Y para desenhar no tabuleiro
const int board_pos_x[] = {15, 19, 23};
const int board_pos_y[] = {7, 9, 11};

// --- VARIÁVEIS DE ESTADO DA PLATAFORMA ---

TicTacToeGame game;       // Instância da estrutura do jogo que contém todo o estado
int cursorPos = 4;        // Posição atual do cursor na tela (0-8)
int lastCursorPos = -1;   // Última posição do cursor para fins de redesenho

// --- FUNÇÕES DE RENDERIZAÇÃO (PLATAFORMA) ---

void drawScoreboard() {
    char placar[40];
    sprintf(placar, "X: %d   O: %d   Empates: %d", game.scoreX, game.scoreO, game.draws);
    VDP_clearText(0, 1, 40);
    int len = strlen(placar);
    int x = (40 - len) / 2;
    VDP_drawText(placar, x, 1);
}

void drawBoard() {
    VDP_drawText("+---+---+---+", 13, 6);
    VDP_drawText("|   |   |   |", 13, 7);
    VDP_drawText("+---+---+---+", 13, 8);
    VDP_drawText("|   |   |   |", 13, 9);
    VDP_drawText("+---+---+---+", 13, 10);
    VDP_drawText("|   |   |   |", 13, 11);
    VDP_drawText("+---+---+---+", 13, 12);
}

void updateCursorVisual() {
    if (lastCursorPos != -1) {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 0), board_pos_x[lastCursorPos % 3], board_pos_y[lastCursorPos / 3]);
    }
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, TILE_CURSOR), board_pos_x[cursorPos % 3], board_pos_y[cursorPos / 3]);
    lastCursorPos = cursorPos;
}

void showEndGameMessage() {
    VDP_clearText(10, 2, 20);
    if (game.status == GAME_STATE_WIN) {
        if (game.winner == CELL_X) {
            VDP_drawText("JOGADOR X VENCEU!", 10, 4);
            PAL_setColor(1, 0x00E0); // Cor verde para vitória do X
        } else {
            VDP_drawText("JOGADOR O VENCEU!", 10, 4);
            PAL_setColor(1, 0x000E); // Cor azul para vitória do O
        }
    } else if (game.status == GAME_STATE_DRAW) {
        VDP_drawText("EMPATE!", 15, 4);
        PAL_setColor(1, 0x0AAA); // Cor cinza para empate
    }
    VDP_drawText("Aperte START para reiniciar", 6, 26);
}

void showPlayerTurn() {
    VDP_clearText(10, 2, 20);
    if (game.currentPlayer == 1) {
        VDP_drawText("Vez do Jogador: X", 10, 2);
    } else {
        VDP_drawText("Vez do Jogador: O", 10, 2);
    }
}

// --- FUNÇÕES DE CONTROLE DE JOGO (PLATAFORMA) ---

void startGame() {
    Game_Reset(&game);
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    drawBoard();
    drawScoreboard();
    cursorPos = 4;
    updateCursorVisual();
    showPlayerTurn();
}

// --- FUNÇÃO PRINCIPAL (main) ---

int main() {
    // Inicialização do hardware do Mega Drive
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();
    VDP_setPlaneSize(64, 32, TRUE);
    VDP_setEnable(1);
    VDP_setBackgroundColor(2);
    PAL_setPalette(PAL1, palette, DMA);
    SPR_init();
    JOY_init();

    // Carrega os tiles para a VRAM
    VDP_loadTileData(x_tile, TILE_X, 1, DMA);
    VDP_loadTileData(o_tile, TILE_O, 1, DMA);
    VDP_loadTileData(cursor_tile, TILE_CURSOR, 1, DMA);

    // Inicializa o jogo pela primeira vez (zera placares)
    Game_Init(&game);
    startGame();

    u16 joy_value, joy_previous_value = 0, joy_pressed;

    while(1) {
        joy_value = JOY_readJoypad(JOY_1);
        joy_pressed = joy_value & (joy_value ^ joy_previous_value);
        joy_previous_value = joy_value;

        if (game.status == GAME_STATE_PLAYING) {
            // Movimentação do cursor
            if (joy_pressed & BUTTON_UP && cursorPos > 2) cursorPos -= 3;
            else if (joy_pressed & BUTTON_DOWN && cursorPos < 6) cursorPos += 3;
            else if (joy_pressed & BUTTON_LEFT && cursorPos % 3 > 0) cursorPos -= 1;
            else if (joy_pressed & BUTTON_RIGHT && cursorPos % 3 < 2) cursorPos += 1;
            
            // Ação de jogar
            else if (joy_pressed & BUTTON_A) {
                int previousPlayer = game.currentPlayer;
                if (Game_MakeMove(&game, cursorPos)) {
                    // Desenha o X ou O na tela
                    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, TILE_X + previousPlayer - 1), board_pos_x[cursorPos % 3], board_pos_y[cursorPos / 3]);
                    
                    if (game.status != GAME_STATE_PLAYING) {
                        showEndGameMessage();
                        drawScoreboard(); // Atualiza placar com a última pontuação
                    } else {
                        showPlayerTurn();
                    }
                }
            }
        } else { // Fim de jogo (GAME_STATE_WIN ou GAME_STATE_DRAW)
            if (joy_pressed & BUTTON_START) {
                PAL_setColor(1, 0x0EEE); // Restaura a cor padrão
                startGame();
            }
        }

        updateCursorVisual();
        SPR_update();
        SYS_doVBlankProcess();
    }
    return 0;
}