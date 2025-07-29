
// Jogo da Velha para Mega Drive usando SGDK
// Este código implementa um jogo da velha jogável no Mega Drive, com gráficos simples e controle pelo joystick.
// Cada parte do código está comentada para servir como tutorial.

#include <genesis.h> // Biblioteca principal do SGDK para desenvolvimento no Mega Drive


// --- RECURSOS GRÁFICOS (TILES) ---
// Tiles são blocos gráficos usados para desenhar X, O e o cursor no tabuleiro.
const u32 x_tile[8] =
{
    0x01100110, 0x00111100, 0x00011000, 0x00111100,
    0x01100110, 0x11000011, 0x10000001, 0x00000000
}; // Tile do X

const u32 o_tile[8] =
{
    0x00111100, 0x01111110, 0x11000011, 0x11000011,
    0x11000011, 0x01111110, 0x00111100, 0x00000000
}; // Tile do O

const u32 cursor_tile[8] =
{
    0x33333333, // linha superior
    0x33000033, // laterais
    0x33000033,
    0x33000033,
    0x33000033,
    0x33000033,
    0x33333333, // linha inferior
    0x00000000  // transparente
}; // Tile do cursor


// --- PALETA DE CORES ---
// Define as cores usadas no jogo (formato RGB do Mega Drive)
const u16 palette[16] =
{
    0x0000, 0x0EEE, 0x0800, 0x0EEA, 0x00E0, 0x000E, 0x0AAA, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};



// --- VARIÁVEIS DE ESTADO DO JOGO ---
// boardState: armazena o estado de cada casa do tabuleiro (0 = vazio, 1 = X, 2 = O)
int boardState[9];
// playerTurn: indica o jogador atual (1 = X, 2 = O)
int playerTurn;
// cursorPos: posição atual do cursor (0 a 8)
int cursorPos;
// gameState: 0 = jogando, 1 = fim de jogo
int gameState;

// Placar/ranking
int scoreX = 0;
int scoreO = 0;
int empates = 0;

// Posições X e Y dos tiles no tabuleiro (ajustadas para centralizar)
const int board_pos_x[] = {15, 19, 23};
const int board_pos_y[] = {7, 9, 11};

// lastCursorPos: armazena a última posição do cursor para apagar o tile anterior
int lastCursorPos = -1;
// Desenha o placar/ranking no topo da tela
void drawScoreboard() {
    char placar[40];
    // Monta a string centralizada (máx 40 colunas)
    sprintf(placar, "X: %d   O: %d   Empates: %d", scoreX, scoreO, empates);
    // Limpa a linha do placar
    VDP_clearText(0, 1, 40);
    // Centraliza o texto (tamanho variável)
    int len = strlen(placar);
    int x = (40 - len) / 2;
    VDP_drawText(placar, x, 1);
}


// --- ÍNDICES DOS TILES ---
// Define os índices dos tiles na VRAM
#define TILE_BASE_INDEX 256
#define TILE_X (TILE_BASE_INDEX)
#define TILE_O (TILE_BASE_INDEX + 1)
#define TILE_CURSOR (TILE_BASE_INDEX + 2)



// --- FUNÇÕES DO JOGO ---


// Desenha o tabuleiro do jogo na tela usando texto (centralizado)
void drawBoard() {
    // O tabuleiro tem 13 colunas de largura, centralizando em X=13
    VDP_drawText("+---+---+---+", 13, 6);
    VDP_drawText("|   |   |   |", 13, 7);
    VDP_drawText("+---+---+---+", 13, 8);
    VDP_drawText("|   |   |   |", 13, 9);
    VDP_drawText("+---+---+---+", 13, 10);
    VDP_drawText("|   |   |   |", 13, 11);
    VDP_drawText("+---+---+---+", 13, 12);
}

// Atualiza a posição do cursor visual no tabuleiro
void updateCursorPos() {
    // Apaga o tile do cursor anterior no plano A
    if (lastCursorPos != -1)
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 0), board_pos_x[lastCursorPos % 3], board_pos_y[lastCursorPos / 3]);
    // Desenha o tile do cursor no plano A
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, TILE_CURSOR), board_pos_x[cursorPos % 3], board_pos_y[cursorPos / 3]);
    lastCursorPos = cursorPos;
}

// Verifica se algum jogador venceu ou se houve empate
// Retorna: 0 = jogo continua, 1 = X venceu, 2 = O venceu, 3 = empate
int checkWin() {
    for(int i = 0; i < 3; i++) {
        // Checa linhas
        if (boardState[i*3] == boardState[i*3+1] && boardState[i*3+1] == boardState[i*3+2] && boardState[i*3] != 0) return boardState[i*3];
        // Checa colunas
        if (boardState[i] == boardState[i+3] && boardState[i+3] == boardState[i+6] && boardState[i] != 0) return boardState[i];
    }
    // Checa diagonais
    if (boardState[0] == boardState[4] && boardState[4] == boardState[8] && boardState[0] != 0) return boardState[0];
    if (boardState[2] == boardState[4] && boardState[4] == boardState[6] && boardState[2] != 0) return boardState[2];
    // Checa se ainda há casas vazias
    for(int i = 0; i < 9; i++) if (boardState[i] == 0) return 0;
    // Se não há vencedor e não há casas vazias, é empate
    return 3;
}

// Inicia ou reinicia o jogo
void startGame() {
    VDP_clearPlane(BG_A, TRUE); // Limpa plano A
    VDP_clearPlane(BG_B, TRUE); // Limpa plano B
    drawBoard(); // Desenha o tabuleiro
    drawScoreboard(); // Desenha o placar
    for(int i = 0; i < 9; i++) boardState[i] = 0; // Limpa o estado do tabuleiro
    playerTurn = 1; // X começa
    cursorPos = 4; // Cursor no centro
    gameState = 0; // Jogo em andamento
    updateCursorPos(); // Atualiza cursor
    VDP_drawText("Vez do Jogador: X", 10, 2); // Mostra mensagem
}



// Função principal do jogo

int main()
{
    // Força o modo NTSC (60Hz) para garantir velocidade e resolução padrão NTSC
    // Isso evita lentidão ou diferenças de tela em consoles PAL
    VDP_setScreenWidth320(); // Garante largura padrão NTSC (opcional, mas recomendado)
    VDP_setScreenHeight224(); // Garante altura padrão NTSC (224 linhas)
    VDP_setPlaneSize(64,32, TRUE); // Tamanho padrão dos planos para 320x224
    VDP_setEnable(1); // Garante que o VDP está habilitado

    // Configura cor de fundo e paleta
    VDP_setBackgroundColor(2);
    PAL_setPalette(PAL1, palette, DMA);

    // Inicializa sprites e joystick
    SPR_init();
    JOY_init();

    // Carrega os tiles gráficos para X, O e cursor
    VDP_loadTileData(x_tile, TILE_X, 1, DMA);
    VDP_loadTileData(o_tile, TILE_O, 1, DMA);
    VDP_loadTileData(cursor_tile, TILE_CURSOR, 1, DMA);

    // Inicia o jogo
    startGame();

    // Variáveis para leitura do joystick
    u16 joy_value = 0;
    u16 joy_previous_value = 0;
    u16 joy_pressed = 0;

    // Loop principal do jogo
    while(1)
    {
        // Lê o estado do joystick
        joy_value = JOY_readJoypad(JOY_1);
        // Detecta botões pressionados neste frame
        joy_pressed = joy_value & (joy_value ^ joy_previous_value);
        joy_previous_value = joy_value;

        if (gameState == 0) { // Jogo em andamento
            // Movimentação do cursor
            if (joy_pressed & BUTTON_UP && cursorPos > 2) cursorPos -= 3;
            else if (joy_pressed & BUTTON_DOWN && cursorPos < 6) cursorPos += 3;
            else if (joy_pressed & BUTTON_LEFT && cursorPos % 3 > 0) cursorPos -= 1;
            else if (joy_pressed & BUTTON_RIGHT && cursorPos % 3 < 2) cursorPos += 1;
            // Jogada
            else if (joy_pressed & BUTTON_A) {
                if (boardState[cursorPos] == 0) {
                    boardState[cursorPos] = playerTurn; // Marca X ou O
                    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, TILE_X + playerTurn - 1), board_pos_x[cursorPos % 3], board_pos_y[cursorPos / 3]);
                    int winner = checkWin(); // Verifica vitória ou empate
                    if (winner != 0) {
                        gameState = 1; // Fim de jogo
                        VDP_clearText(10, 2, 20);
                        // Atualiza placar
                        if (winner == 1) { scoreX++; VDP_drawText("JOGADOR X VENCEU!", 10, 4); PAL_setColor(1, 0x00E0); }
                        else if (winner == 2) { scoreO++; VDP_drawText("JOGADOR O VENCEU!", 10, 4); PAL_setColor(1, 0x000E); }
                        else { empates++; VDP_drawText("EMPATE!", 15, 4); PAL_setColor(1, 0x0AAA); }
                        drawScoreboard();
                        VDP_drawText("Aperte START para reiniciar", 6, 26);
                    } else {
                        playerTurn = (playerTurn == 1) ? 2 : 1; // Alterna jogador
                        VDP_clearText(10, 2, 20);
                        if (playerTurn == 1) VDP_drawText("Vez do Jogador: X", 10, 2); else VDP_drawText("Vez do Jogador: O", 10, 2);
                    }
                }
            }
        } else { // Fim de jogo
            // Reinicia o jogo ao apertar START
            if (joy_pressed & BUTTON_START) {
                PAL_setColor(1, 0x0EEE);
                startGame();
            }
        }

        updateCursorPos(); // Atualiza cursor visual
        SPR_update();      // Atualiza sprites (caso use)
        SYS_doVBlankProcess(); // Sincroniza com o VBlank
    }
    return (0);
}