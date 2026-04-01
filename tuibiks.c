#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

typedef enum {
    WHITE,
    ORANGE,
    GREEN,
    RED,
    BLUE,
    YELLOW
} Color;

typedef struct {
    Color up[9];
    Color left[9];
    Color front[9];
    Color right[9];
    Color back[9];
    Color down[9];
} Cube;

void cube_init(Cube *cube) {
    // Initialize cube in solved state

    for (int i = 0; i < 9; i++) {
        cube->up[i] = WHITE;
        cube->left[i] = ORANGE;
        cube->front[i] = GREEN;
        cube->right[i] = RED;
        cube->back[i] = BLUE;
        cube->down[i] = YELLOW;
    }
}

#define FACE_IDX(row, col) (3 * (row) + (col))


////////////////////////////////////////
// Face Rotations
////////////////////////////////////////

void face_rotate_cw(Color face[9]) {
    // Rotate face clockwise
    //
    // F6 F3 F0 <- F0 F1 F2
    // F7 F4 F1 <- F3 F4 F5
    // F8 F5 F2 <- F6 F7 F8

    Color temp[9];
    memcpy(temp, face, sizeof(temp));
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int new_row = col;
            int new_col = 2 - row;
            face[FACE_IDX(new_row, new_col)] = temp[FACE_IDX(row, col)];
        }
    }
}

void face_rotate_ccw(Color face[9]) {
    // Rotate face counterclockwise
    //
    // F2 F5 F8 <- F0 F1 F2
    // F1 F4 F7 <- F3 F4 F5
    // F0 F3 F6 <- F6 F7 F8

    Color temp[9];
    memcpy(temp, face, sizeof(temp));
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int new_row = 2 - col;
            int new_col = row;
            face[FACE_IDX(new_row, new_col)] = temp[FACE_IDX(row, col)];
        }
    }
}


////////////////////////////////////////
// Cube Face Rotations
////////////////////////////////////////

void cube_rotate_u(Cube *cube) {
    // Rotate up clockwise

    face_rotate_cw(cube->up);

    // B <- L <- F <- R <- B (top row)
    for (int col = 0; col < 3; col++) {
        Color temp = cube->back[FACE_IDX(0, col)];
        cube->back[FACE_IDX(0, col)] = cube->left[FACE_IDX(0, col)];
        cube->left[FACE_IDX(0, col)] = cube->front[FACE_IDX(0, col)];
        cube->front[FACE_IDX(0, col)] = cube->right[FACE_IDX(0, col)];
        cube->right[FACE_IDX(0, col)] = temp;
    }
}

void cube_rotate_d(Cube *cube) {
    // Rotate down clockwise

    face_rotate_cw(cube->down);

    // L <- B <- R <- F <- L (bottom row)
    for (int col = 0; col < 3; col++) {
        Color temp = cube->left[FACE_IDX(2, col)];
        cube->left[FACE_IDX(2, col)] = cube->back[FACE_IDX(2, col)];
        cube->back[FACE_IDX(2, col)] = cube->right[FACE_IDX(2, col)];
        cube->right[FACE_IDX(2, col)] = cube->front[FACE_IDX(2, col)];
        cube->front[FACE_IDX(2, col)] = temp;
    }
}

void cube_rotate_l(Cube *cube) {
    // Rotate left clockwise

    face_rotate_cw(cube->left);

    // D <- F <- U (left column)
    Color temp[3];
    for (int row = 0; row < 3; row++) {
        temp[row] = cube->down[FACE_IDX(row, 0)];
        cube->down[FACE_IDX(row, 0)] = cube->front[FACE_IDX(row, 0)];
        cube->front[FACE_IDX(row, 0)] = cube->up[FACE_IDX(row, 0)];
    }

    // U <- B rotated 180
    // U6 <- B2
    // U3 <- B5
    // U0 <- B8
    for (int row = 0; row < 3; row++) {
        cube->up[FACE_IDX(2 - row, 0)] = cube->back[FACE_IDX(row, 2)];
    }

    // B <- D rotated 180
    // B8 <- D0
    // B5 <- D3
    // B2 <- D6
    for (int row = 0; row < 3; row++) {
        cube->back[FACE_IDX(2 - row, 2)] = temp[row];
    }
}

void cube_rotate_r(Cube *cube) {
    // Rotate right clockwise

    face_rotate_cw(cube->right);

    // U <- F <- D (right column)
    Color temp[3];
    for (int row = 0; row < 3; row++) {
        temp[row] = cube->up[FACE_IDX(row, 2)];
        cube->up[FACE_IDX(row, 2)] = cube->front[FACE_IDX(row, 2)];
        cube->front[FACE_IDX(row, 2)] = cube->down[FACE_IDX(row, 2)];
    }

    // D <- B rotated 180
    // D8 <- B0
    // D5 <- B3
    // D2 <- B6
    for (int row = 0; row < 3; row++) {
        cube->down[FACE_IDX(2 - row, 2)] = cube->back[FACE_IDX(row, 0)];
    }

    // B <- U rotated 180
    // B6 <- U2
    // B3 <- U5
    // B0 <- U8
    for (int row = 0; row < 3; row++) {
        cube->back[FACE_IDX(2 - row, 0)] = temp[row];
    }
}

void cube_rotate_f(Cube *cube) {
    // Rotate front clockwise

    face_rotate_cw(cube->front);

    // D2 <- R0
    // D1 <- R3
    // D0 <- R6
    Color temp[3];
    memcpy(temp, cube->down, sizeof(temp));
    for (int row = 0; row < 3; row++) {
        int new_col = 2 - row;
        cube->down[FACE_IDX(0, new_col)] = cube->right[FACE_IDX(row, 0)];
    }

    // R0 <- U6
    // R3 <- U7
    // R6 <- U8
    for (int col = 0; col < 3; col++) {
        int new_row = col;
        cube->right[FACE_IDX(new_row, 0)] = cube->up[FACE_IDX(2, col)];
    }

    // U8 <- L2
    // U7 <- L5
    // U6 <- L8
    for (int row = 0; row < 3; row++) {
        int new_col = 2 - row;
        cube->up[FACE_IDX(2, new_col)] = cube->left[FACE_IDX(row, 2)];
    }

    // L2 <- D0
    // L5 <- D1
    // L8 <- D2
    for (int col = 0; col < 3; col++) {
        int new_row = col;
        cube->left[FACE_IDX(new_row, 2)] = temp[col];
    }
}

void cube_rotate_b(Cube *cube) {
    // Rotate back clockwise

    face_rotate_cw(cube->back);

    // U0 <- R2
    // U1 <- R5
    // U2 <- R8
    Color temp[3];
    memcpy(temp, cube->up, sizeof(temp));
    for (int row = 0; row < 3; row++) {
        int new_col = row;
        cube->up[FACE_IDX(0, new_col)] = cube->right[FACE_IDX(row, 2)];
    }

    // R8 <- D6
    // R5 <- D7
    // R2 <- D8
    for (int col = 0; col < 3; col++) {
        int new_row = 2 - col;
        cube->right[FACE_IDX(new_row, 2)] = cube->down[FACE_IDX(2, col)];
    }

    // D8 <- L0
    // D7 <- L3
    // D6 <- L6
    for (int row = 0; row < 3; row++) {
        int new_col = row;
        cube->down[FACE_IDX(2, new_col)] = cube->left[FACE_IDX(row, 0)];
    }

    // L6 <- U0
    // L3 <- U1
    // L0 <- U2
    for (int col = 0; col < 3; col++) {
        int new_row = 2 - col;
        cube->left[FACE_IDX(new_row, 0)] = temp[col];
    }
}


////////////////////////////////////////
// Cube Face Rotations Inverted
////////////////////////////////////////

void cube_rotate_ui(Cube *cube) {
    // Rotate up counterclockwise

    face_rotate_ccw(cube->up);

    // B <- R <- F <- L <- B (top row)
    for (int col = 0; col < 3; col++) {
        Color temp = cube->back[FACE_IDX(0, col)];
        cube->back[FACE_IDX(0, col)] = cube->right[FACE_IDX(0, col)];
        cube->right[FACE_IDX(0, col)] = cube->front[FACE_IDX(0, col)];
        cube->front[FACE_IDX(0, col)] = cube->left[FACE_IDX(0, col)];
        cube->left[FACE_IDX(0, col)] = temp;
    }
}

void cube_rotate_di(Cube *cube) {
    // Rotate down counterclockwise

    face_rotate_ccw(cube->down);

    // L <- F <- R <- B <- L (bottom row)
    for (int col = 0; col < 3; col++) {
        Color temp = cube->left[FACE_IDX(2, col)];
        cube->left[FACE_IDX(2, col)] = cube->front[FACE_IDX(2, col)];
        cube->front[FACE_IDX(2, col)] = cube->right[FACE_IDX(2, col)];
        cube->right[FACE_IDX(2, col)] = cube->back[FACE_IDX(2, col)];
        cube->back[FACE_IDX(2, col)] = temp;
    }
}

void cube_rotate_li(Cube *cube) {
    // Rotate left counterclockwise

    face_rotate_ccw(cube->left);

    // U <- F <- D (left column)
    Color temp[3];
    for (int row = 0; row < 3; row++) {
        temp[row] = cube->up[FACE_IDX(row, 0)];
        cube->up[FACE_IDX(row, 0)] = cube->front[FACE_IDX(row, 0)];
        cube->front[FACE_IDX(row, 0)] = cube->down[FACE_IDX(row, 0)];
    }

    // D <- B rotated 180
    // D6 <- B2
    // D3 <- B5
    // D0 <- B8
    for (int row = 0; row < 3; row++) {
        cube->down[FACE_IDX(2 - row, 0)] = cube->back[FACE_IDX(row, 2)];
    }

    // B <- U rotated 180
    // B8 <- U0
    // B5 <- U3
    // B2 <- U6
    for (int row = 0; row < 3; row++) {
        cube->back[FACE_IDX(2 - row, 2)] = temp[row];
    }
}

void cube_rotate_ri(Cube *cube) {
    // Rotate right counterclockwise

    face_rotate_ccw(cube->right);

    // D <- F <- U (right column)
    Color temp[3];
    for (int row = 0; row < 3; row++) {
        temp[row] = cube->down[FACE_IDX(row, 2)];
        cube->down[FACE_IDX(row, 2)] = cube->front[FACE_IDX(row, 2)];
        cube->front[FACE_IDX(row, 2)] = cube->up[FACE_IDX(row, 2)];
    }

    // U <- B rotated 180
    // U8 <- B0
    // U5 <- B3
    // U2 <- B6
    for (int row = 0; row < 3; row++) {
        cube->up[FACE_IDX(2 - row, 2)] = cube->back[FACE_IDX(row, 0)];
    }

    // B <- D rotated 180
    // B6 <- D2
    // B3 <- D5
    // B0 <- D8
    for (int row = 0; row < 3; row++) {
        cube->back[FACE_IDX(2 - row, 0)] = temp[row];
    }
}

void cube_rotate_fi(Cube *cube) {
    // Rotate front counterclockwise

    face_rotate_ccw(cube->front);

    // D0 <- L2
    // D1 <- L5
    // D2 <- L8
    Color temp[3];
    memcpy(temp, cube->down, sizeof(temp));
    for (int row = 0; row < 3; row++) {
        int new_col = row;
        cube->down[FACE_IDX(0, new_col)] = cube->left[FACE_IDX(row, 2)];
    }

    // L8 <- U6
    // L5 <- U7
    // L2 <- U8
    for (int col = 0; col < 3; col++) {
        int new_row = 2 - col;
        cube->left[FACE_IDX(new_row, 2)] = cube->up[FACE_IDX(2, col)];
    }

    // U6 <- R0
    // U7 <- R3
    // U8 <- R6
    for (int row = 0; row < 3; row++) {
        int new_col = row;
        cube->up[FACE_IDX(2, new_col)] = cube->right[FACE_IDX(row, 0)];
    }

    // R6 <- D0
    // R3 <- D1
    // R0 <- D2
    for (int col = 0; col < 3; col++) {
        int new_row = 2 - col;
        cube->right[FACE_IDX(new_row, 0)] = temp[col];
    }
}

void cube_rotate_bi(Cube *cube) {
    // Rotate back counterclockwise

    face_rotate_ccw(cube->back);

    // U2 <- L0
    // U1 <- L3
    // U0 <- L6
    Color temp[3];
    memcpy(temp, cube->up, sizeof(temp));
    for (int row = 0; row < 3; row++) {
        // int new_row = col;
        int new_col = 2 - row;
        cube->up[FACE_IDX(0, new_col)] = cube->left[FACE_IDX(row, 0)];
    }

    // L0 <- D6
    // L3 <- D7
    // L6 <- D8
    for (int col = 0; col < 3; col++) {
        int new_row = col;
        cube->left[FACE_IDX(new_row, 0)] = cube->down[FACE_IDX(2, col)];
    }

    // D8 <- R2
    // D7 <- L5
    // D6 <- L8
    for (int row = 0; row < 3; row++) {
        int new_col = 2 - row;
        cube->down[FACE_IDX(2, new_col)] = cube->right[FACE_IDX(row, 2)];
    }

    // R2 <- U0
    // R5 <- U1
    // R8 <- U2
    for (int col = 0; col < 3; col++) {
        int new_row = col;
        cube->right[FACE_IDX(new_row, 2)] = temp[col];
    }
}


////////////////////////////////////////
// Whole Cube Rotations
////////////////////////////////////////

void cube_rotate_x(Cube *cube) {
    // Rotate the entire cube on R

    face_rotate_cw(cube->right);

    face_rotate_ccw(cube->left);

    // U <- F <- D
    Color temp[9];
    memcpy(temp, cube->up, sizeof(temp));
    memcpy(cube->up, cube->front, sizeof(cube->up));
    memcpy(cube->front, cube->down, sizeof(cube->front));

    // D <- B rotated 180
    for (int i = 0; i < 9; i++) {
        cube->down[8 - i] = cube->back[i];
     }

    // B <- U rotated 180
    for (int i = 0; i < 9; i++) {
        cube->back[8 - i] = temp[i];
    }
}

void cube_rotate_y(Cube *cube) {
    // Rotate the entire cube on U

    face_rotate_cw(cube->up);

    face_rotate_ccw(cube->down);

    // B <- L <- F <- R <- B
    Color temp[9];
    memcpy(temp, cube->back, sizeof(temp));
    memcpy(cube->back, cube->left, sizeof(cube->back));
    memcpy(cube->left, cube->front, sizeof(cube->left));
    memcpy(cube->front, cube->right, sizeof(cube->front));
    memcpy(cube->right, temp, sizeof(cube->right));
}

void cube_rotate_z(Cube *cube) {
    // Rotate the entire cube on F

    face_rotate_cw(cube->front);

    face_rotate_ccw(cube->back);

    // L <- D <- R <- U <- L
    // BUT ALL ROTATED CLOCKWISE!
    Color temp[9];
    memcpy(temp, cube->left, sizeof(temp));
    memcpy(cube->left, cube->down, sizeof(cube->left));
    memcpy(cube->down, cube->right, sizeof(cube->down));
    memcpy(cube->right, cube->up, sizeof(cube->right));
    memcpy(cube->up, temp, sizeof(cube->up));

    face_rotate_cw(cube->left);
    face_rotate_cw(cube->down);
    face_rotate_cw(cube->right);
    face_rotate_cw(cube->up);
}


////////////////////////////////////////
// Whole Cube Rotations Inverted
////////////////////////////////////////

void cube_rotate_xi(Cube *cube) {
    // Rotate the entire cube on L

    face_rotate_cw(cube->left);

    face_rotate_ccw(cube->right);

    // D <- F <- U
    Color temp[9];
    memcpy(temp, cube->down, sizeof(temp));
    memcpy(cube->down, cube->front, sizeof(cube->down));
    memcpy(cube->front, cube->up, sizeof(cube->front));

    // U <- B rotated 180
    for (int i = 0; i < 9; i++) {
        cube->up[8 - i] = cube->back[i];
     }

    // B <- D rotated 180
    for (int i = 0; i < 9; i++) {
        cube->back[8 - i] = temp[i];
    }
}

void cube_rotate_yi(Cube *cube) {
    // Rotate the entire cube on D

    face_rotate_cw(cube->down);

    face_rotate_ccw(cube->up);

    // B <- R <- F <- L <- B
    Color temp[9];
    memcpy(temp, cube->back, sizeof(temp));
    memcpy(cube->back, cube->right, sizeof(cube->back));
    memcpy(cube->right, cube->front, sizeof(cube->right));
    memcpy(cube->front, cube->left, sizeof(cube->front));
    memcpy(cube->left, temp, sizeof(cube->left));
}

void cube_rotate_zi(Cube *cube) {
    // Rotate the entire cube on B

    face_rotate_cw(cube->back);

    face_rotate_ccw(cube->front);

    // L <- U <- R <- D <- L
    // BUT ALL ROTATED COUNTERCLOCKWISE!
    Color temp[9];
    memcpy(temp, cube->left, sizeof(temp));
    memcpy(cube->left, cube->up, sizeof(cube->left));
    memcpy(cube->up, cube->right, sizeof(cube->up));
    memcpy(cube->right, cube->down, sizeof(cube->right));
    memcpy(cube->down, temp, sizeof(cube->down));

    face_rotate_ccw(cube->left);
    face_rotate_ccw(cube->up);
    face_rotate_ccw(cube->right);
    face_rotate_ccw(cube->down);
}


////////////////////////////////////////
// Moves (Face Rotations)
////////////////////////////////////////

typedef enum {
    MOVE_U,
    MOVE_D,
    MOVE_L,
    MOVE_R,
    MOVE_F,
    MOVE_B,

    MOVE_UI,
    MOVE_DI,
    MOVE_LI,
    MOVE_RI,
    MOVE_FI,
    MOVE_BI,
} Move;

void cube_rotate(Cube *cube, Move move) {
    switch (move) {
        case MOVE_U:
            cube_rotate_u(cube);
            break;
        case MOVE_D:
            cube_rotate_d(cube);
            break;
        case MOVE_L:
            cube_rotate_l(cube);
            break;
        case MOVE_R:
            cube_rotate_r(cube);
            break;
        case MOVE_F:
            cube_rotate_f(cube);
            break;
        case MOVE_B:
            cube_rotate_b(cube);
            break;

        case MOVE_UI:
            cube_rotate_ui(cube);
            break;
        case MOVE_DI:
            cube_rotate_di(cube);
            break;
        case MOVE_LI:
            cube_rotate_li(cube);
            break;
        case MOVE_RI:
            cube_rotate_ri(cube);
            break;
        case MOVE_FI:
            cube_rotate_fi(cube);
            break;
        case MOVE_BI:
            cube_rotate_bi(cube);
            break;
    }
}


////////////////////////////////////////
// Cube Scramble
////////////////////////////////////////

Move random_move(void) {
    return (Move)(rand() % 12);
}

void cube_scramble(Cube *cube) {
    // TODO: Improve scramble to avoid redundant moves, etc.

    int moves = 25;

    while (moves--) {
        cube_rotate(cube, random_move());
    }
}


////////////////////////////////////////
// Terminal
////////////////////////////////////////

struct termios orig_termios;

void term_restore(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void term_init(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);

    atexit(term_restore);

    struct termios new = orig_termios;
    new.c_lflag &= ~(ECHO | ICANON);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new);
}


////////////////////////////////////////
// Rendering
////////////////////////////////////////

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGBColor;

RGBColor colors[] = {
    {255, 255, 255},  // WHITE
    {255, 88, 0},     // ORANGE
    {0, 155, 72},     // GREEN
    {183, 18, 52},    // RED
    {0, 70, 173},     // BLUE
    {255, 213, 0},    // YELLOW
};

void render_cube_net(const Cube *cube) {
    // Render flattened cube for debugging
    //
    //       U U U
    //       U U U
    //       U U U
    // L L L F F F R R R B B B
    // L L L F F F R R R B B B
    // L L L F F F R R R B B B
    //       D D D
    //       D D D
    //       D D D

    for (int row = 0; row < 3; row++) {
        printf("      ");
        for (int col = 0; col < 3; col++) {
            printf(
                "\x1b[38;2;%d;%d;%dm",
                colors[cube->up[FACE_IDX(row, col)]].r,
                colors[cube->up[FACE_IDX(row, col)]].g,
                colors[cube->up[FACE_IDX(row, col)]].b
            );
            printf("U ");
        }
        printf("\x1b[0m\n");
    }

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            printf(
                "\x1b[38;2;%d;%d;%dm",
                colors[cube->left[FACE_IDX(row, col)]].r,
                colors[cube->left[FACE_IDX(row, col)]].g,
                colors[cube->left[FACE_IDX(row, col)]].b
            );
            printf("L ");
        }
        for (int col = 0; col < 3; col++) {
            printf(
                "\x1b[38;2;%d;%d;%dm",
                colors[cube->front[FACE_IDX(row, col)]].r,
                colors[cube->front[FACE_IDX(row, col)]].g,
                colors[cube->front[FACE_IDX(row, col)]].b
            );
            printf("F ");
        }
        for (int col = 0; col < 3; col++) {
            printf(
                "\x1b[38;2;%d;%d;%dm",
                colors[cube->right[FACE_IDX(row, col)]].r,
                colors[cube->right[FACE_IDX(row, col)]].g,
                colors[cube->right[FACE_IDX(row, col)]].b
            );
            printf("R ");
        }
        for (int col = 0; col < 3; col++) {
            printf(
                "\x1b[38;2;%d;%d;%dm",
                colors[cube->back[FACE_IDX(row, col)]].r,
                colors[cube->back[FACE_IDX(row, col)]].g,
                colors[cube->back[FACE_IDX(row, col)]].b
            );
            printf("B ");
        }
        printf("\x1b[0m\n");
    }

    for (int row = 0; row < 3; row++) {
        printf("      ");
        for (int col = 0; col < 3; col++) {
            printf(
                "\x1b[38;2;%d;%d;%dm",
                colors[cube->down[FACE_IDX(row, col)]].r,
                colors[cube->down[FACE_IDX(row, col)]].g,
                colors[cube->down[FACE_IDX(row, col)]].b
            );
            printf("D ");
        }
        printf("\x1b[0m\n");
    }
}

void render_cube(const Cube *cube) {
    // Render 3D cube
    //
    //       🭈🭆🭂🭍🭑🬽
    //    🭈🭆🭂🭍🭑🬽🭈🭆🭂🭍🭑🬽
    // 🭈🭆🭂🭍🭑🬽🭈🭆🭂🭍🭑🬽🭈🭆🭂🭍🭑🬽
    // 🭍🭑🬽🭈🭆🭂🭍🭑🬽🭈🭆🭂🭍🭑🬽🭈🭆🭂
    // ███🭍🭑🬽🭈🭆🭂🭍🭑🬽🭈🭆🭂███
    // 🭍🭑🬽███🭍🭑🬽🭈🭆🭂███🭈🭆🭂
    // ███🭍🭑🬽██████🭈🭆🭂███
    // 🭍🭑🬽███🭍🭑🬽🭈🭆🭂███🭈🭆🭂
    // ███🭍🭑🬽██████🭈🭆🭂███
    // 🭣🭧🭓███🭍🭑🬽🭈🭆🭂███🭞🭜🭘
    //    🭣🭧🭓██████🭞🭜🭘
    //       🭣🭧🭓🭞🭜🭘

    printf("   ");
    printf("   ");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[0]].r,
        colors[cube->up[0]].g,
        colors[cube->up[0]].b
    );
    printf("🭈🭆🭂");
    printf("🭍🭑🬽");
    printf("\x1b[0m\n");

    printf("   ");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[3]].r,
        colors[cube->up[3]].g,
        colors[cube->up[3]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->up[0]].r,
        colors[cube->up[0]].g,
        colors[cube->up[0]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[1]].r,
        colors[cube->up[1]].g,
        colors[cube->up[1]].b
    );
    printf("🭈🭆🭂");
    printf("\x1b[0m");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[1]].r,
        colors[cube->up[1]].g,
        colors[cube->up[1]].b
    );
    printf("🭍🭑🬽");
    printf("\x1b[0m\n");

    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[6]].r,
        colors[cube->up[6]].g,
        colors[cube->up[6]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->up[3]].r,
        colors[cube->up[3]].g,
        colors[cube->up[3]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[4]].r,
        colors[cube->up[4]].g,
        colors[cube->up[4]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->up[1]].r,
        colors[cube->up[1]].g,
        colors[cube->up[1]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[2]].r,
        colors[cube->up[2]].g,
        colors[cube->up[2]].b
    );
    printf("🭈🭆🭂");
    printf("\x1b[0m");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[2]].r,
        colors[cube->up[2]].g,
        colors[cube->up[2]].b
    );
    printf("🭍🭑🬽");
    printf("\x1b[0m\n");

    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->up[6]].r,
        colors[cube->up[6]].g,
        colors[cube->up[6]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[0]].r,
        colors[cube->front[0]].g,
        colors[cube->front[0]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[7]].r,
        colors[cube->up[7]].g,
        colors[cube->up[7]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->up[4]].r,
        colors[cube->up[4]].g,
        colors[cube->up[4]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[5]].r,
        colors[cube->up[5]].g,
        colors[cube->up[5]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->up[2]].r,
        colors[cube->up[2]].g,
        colors[cube->up[2]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[2]].r,
        colors[cube->right[2]].g,
        colors[cube->right[2]].b
    );
    printf("🭈🭆🭂");
    printf("\x1b[0m\n");

    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[0]].r,
        colors[cube->front[0]].g,
        colors[cube->front[0]].b
    );
    printf("███");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->up[7]].r,
        colors[cube->up[7]].g,
        colors[cube->up[7]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[1]].r,
        colors[cube->front[1]].g,
        colors[cube->front[1]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->up[8]].r,
        colors[cube->up[8]].g,
        colors[cube->up[8]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->up[5]].r,
        colors[cube->up[5]].g,
        colors[cube->up[5]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[1]].r,
        colors[cube->right[1]].g,
        colors[cube->right[1]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[2]].r,
        colors[cube->right[2]].g,
        colors[cube->right[2]].b
    );
    printf("███");
    printf("\x1b[0m\n");

    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->front[0]].r,
        colors[cube->front[0]].g,
        colors[cube->front[0]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[3]].r,
        colors[cube->front[3]].g,
        colors[cube->front[3]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[1]].r,
        colors[cube->front[1]].g,
        colors[cube->front[1]].b
    );
    printf("███");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->up[8]].r,
        colors[cube->up[8]].g,
        colors[cube->up[8]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[2]].r,
        colors[cube->front[2]].g,
        colors[cube->front[2]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[0]].r,
        colors[cube->right[0]].g,
        colors[cube->right[0]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[1]].r,
        colors[cube->right[1]].g,
        colors[cube->right[1]].b
    );
    printf("███");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->right[2]].r,
        colors[cube->right[2]].g,
        colors[cube->right[2]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[5]].r,
        colors[cube->right[5]].g,
        colors[cube->right[5]].b
    );
    printf("🭈🭆🭂");
    printf("\x1b[0m\n");

    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[3]].r,
        colors[cube->front[3]].g,
        colors[cube->front[3]].b
    );
    printf("███");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->front[1]].r,
        colors[cube->front[1]].g,
        colors[cube->front[1]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[4]].r,
        colors[cube->front[4]].g,
        colors[cube->front[4]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[2]].r,
        colors[cube->front[2]].g,
        colors[cube->front[2]].b
    );
    printf("███");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[0]].r,
        colors[cube->right[0]].g,
        colors[cube->right[0]].b
    );
    printf("███");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->right[1]].r,
        colors[cube->right[1]].g,
        colors[cube->right[1]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[4]].r,
        colors[cube->right[4]].g,
        colors[cube->right[4]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[5]].r,
        colors[cube->right[5]].g,
        colors[cube->right[5]].b
    );
    printf("███");
    printf("\x1b[0m\n");

    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->front[3]].r,
        colors[cube->front[3]].g,
        colors[cube->front[3]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[6]].r,
        colors[cube->front[6]].g,
        colors[cube->front[6]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[4]].r,
        colors[cube->front[4]].g,
        colors[cube->front[4]].b
    );
    printf("███");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->front[2]].r,
        colors[cube->front[2]].g,
        colors[cube->front[2]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[5]].r,
        colors[cube->front[5]].g,
        colors[cube->front[5]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->right[0]].r,
        colors[cube->right[0]].g,
        colors[cube->right[0]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[3]].r,
        colors[cube->right[3]].g,
        colors[cube->right[3]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[4]].r,
        colors[cube->right[4]].g,
        colors[cube->right[4]].b
    );
    printf("███");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->right[5]].r,
        colors[cube->right[5]].g,
        colors[cube->right[5]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[8]].r,
        colors[cube->right[8]].g,
        colors[cube->right[8]].b
    );
    printf("🭈🭆🭂");
    printf("\x1b[0m\n");

    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[6]].r,
        colors[cube->front[6]].g,
        colors[cube->front[6]].b
    );
    printf("███");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->front[4]].r,
        colors[cube->front[4]].g,
        colors[cube->front[4]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[7]].r,
        colors[cube->front[7]].g,
        colors[cube->front[7]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[5]].r,
        colors[cube->front[5]].g,
        colors[cube->front[5]].b
    );
    printf("███");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[3]].r,
        colors[cube->right[3]].g,
        colors[cube->right[3]].b
    );
    printf("███");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->right[4]].r,
        colors[cube->right[4]].g,
        colors[cube->right[4]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[7]].r,
        colors[cube->right[7]].g,
        colors[cube->right[7]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[8]].r,
        colors[cube->right[8]].g,
        colors[cube->right[8]].b
    );
    printf("███");
    printf("\x1b[0m\n");

    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[6]].r,
        colors[cube->front[6]].g,
        colors[cube->front[6]].b
    );
    printf("🭣🭧🭓");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[7]].r,
        colors[cube->front[7]].g,
        colors[cube->front[7]].b
    );
    printf("███");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->front[5]].r,
        colors[cube->front[5]].g,
        colors[cube->front[5]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[8]].r,
        colors[cube->front[8]].g,
        colors[cube->front[8]].b
    );
    printf("🭍🭑🬽");
    printf(
        "\x1b[48;2;%d;%d;%dm",
        colors[cube->right[3]].r,
        colors[cube->right[3]].g,
        colors[cube->right[3]].b
    );
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[6]].r,
        colors[cube->right[6]].g,
        colors[cube->right[6]].b
    );
    printf("🭈🭆🭂");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[7]].r,
        colors[cube->right[7]].g,
        colors[cube->right[7]].b
    );
    printf("███");
    printf("\x1b[0m");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[8]].r,
        colors[cube->right[8]].g,
        colors[cube->right[8]].b
    );
    printf("🭞🭜🭘");
    printf("\x1b[0m\n");

    printf("   ");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[7]].r,
        colors[cube->front[7]].g,
        colors[cube->front[7]].b
    );
    printf("🭣🭧🭓");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[8]].r,
        colors[cube->front[8]].g,
        colors[cube->front[8]].b
    );
    printf("███");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[6]].r,
        colors[cube->right[6]].g,
        colors[cube->right[6]].b
    );
    printf("███");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[7]].r,
        colors[cube->right[7]].g,
        colors[cube->right[7]].b
    );
    printf("🭞🭜🭘");
    printf("\x1b[0m\n");

    printf("   ");
    printf("   ");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->front[8]].r,
        colors[cube->front[8]].g,
        colors[cube->front[8]].b
    );
    printf("🭣🭧🭓");
    printf(
        "\x1b[38;2;%d;%d;%dm",
        colors[cube->right[6]].r,
        colors[cube->right[6]].g,
        colors[cube->right[6]].b
    );
    printf("🭞🭜🭘");
    printf("\x1b[0m\n");
}

int main() {
    term_init();

    Cube cube;
    cube_init(&cube);

    srand(time(NULL));
    cube_scramble(&cube);

    render_cube(&cube);

    char move;
    while (1) {
        if (read(STDIN_FILENO, &move, 1) != 1) {
            exit(1);
        }

        switch (move) {
            // Face rotations
            case 'U':
                cube_rotate_u(&cube);
                break;
            case 'u':
                cube_rotate_ui(&cube);
                break;
            case 'D':
                cube_rotate_d(&cube);
                break;
            case 'd':
                cube_rotate_di(&cube);
                break;
            case 'L':
                cube_rotate_l(&cube);
                break;
            case 'l':
                cube_rotate_li(&cube);
                break;
            case 'R':
                cube_rotate_r(&cube);
                break;
            case 'r':
                cube_rotate_ri(&cube);
                break;
            case 'F':
                cube_rotate_f(&cube);
                break;
            case 'f':
                cube_rotate_fi(&cube);
                break;
            case 'B':
                cube_rotate_b(&cube);
                break;
            case 'b':
                cube_rotate_bi(&cube);
                break;

            // Whole cube rotations
            case 'X':
                cube_rotate_x(&cube);
                break;
            case 'x':
                cube_rotate_xi(&cube);
                break;
            case 'Y':
                cube_rotate_y(&cube);
                break;
            case 'y':
                cube_rotate_yi(&cube);
                break;
            case 'Z':
                cube_rotate_z(&cube);
                break;
            case 'z':
                cube_rotate_zi(&cube);
                break;

            // Not a move
            default:
                continue;
        }

        printf("\x1b[12A");
        render_cube(&cube);
    }
}
