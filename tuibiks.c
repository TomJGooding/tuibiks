#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    WHITE,
    GREEN,
    RED,
    BLUE,
    ORANGE,
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

#define FACE_IDX(row, col) (3 * (row) + (col))

void face_rotate_cw(Color face[9]) {
    /*
     * F6 F3 F0 <- F0 F1 F2
     * F7 F4 F1 <- F3 F4 F5
     * F8 F5 F2 <- F6 F7 F8
     */

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
    /*
     * F2 F5 F8 <- F0 F1 F2
     * F1 F4 F7 <- F3 F4 F5
     * F0 F3 F6 <- F6 F7 F8
     */

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
// Face Rotations
////////////////////////////////////////

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

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGBColor;

RGBColor colors[] = {
    {255, 255, 255},  // WHITE
    {0, 155, 72},     // GREEN
    {183, 18, 52},    // RED
    {0, 70, 173},     // BLUE
    {255, 88, 0},     // ORANGE
    {255, 213, 0},    // YELLOW
};

void render_cube_net(const Cube *cube) {
    /*
     *       U U U
     *       U U U
     *       U U U
     * L L L F F F R R R B B B
     * L L L F F F R R R B B B
     * L L L F F F R R R B B B
     *       D D D
     *       D D D
     *       D D D
     */

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
    /*
     *       🭈🭆🭂🭍🭑🬽
     *    🭈🭆🭂🭍🭑🬽🭈🭆🭂🭍🭑🬽
     * 🭈🭆🭂🭍🭑🬽🭈🭆🭂🭍🭑🬽🭈🭆🭂🭍🭑🬽
     * 🭍🭑🬽🭈🭆🭂🭍🭑🬽🭈🭆🭂🭍🭑🬽🭈🭆🭂
     * ███🭍🭑🬽🭈🭆🭂🭍🭑🬽🭈🭆🭂███
     * 🭍🭑🬽███🭍🭑🬽🭈🭆🭂███🭈🭆🭂
     * ███🭍🭑🬽██████🭈🭆🭂███
     * 🭍🭑🬽███🭍🭑🬽🭈🭆🭂███🭈🭆🭂
     * ███🭍🭑🬽██████🭈🭆🭂███
     * 🭣🭧🭓███🭍🭑🬽🭈🭆🭂███🭞🭜🭘
     *    🭣🭧🭓██████🭞🭜🭘
     *       🭣🭧🭓🭞🭜🭘
     */

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
    // Cube test_cube = {
    //     .up = {RED, GREEN, BLUE, BLUE, YELLOW, GREEN, GREEN, BLUE, RED},
    //     .left = {RED, BLUE, GREEN, BLUE, YELLOW, RED, GREEN, RED, BLUE},
    //     .front = {GREEN, BLUE, RED, RED, YELLOW, BLUE, BLUE, RED, GREEN},
    //     .right = {RED, GREEN, BLUE, BLUE, YELLOW, GREEN, GREEN, BLUE, RED},
    //     .back = {BLUE, GREEN, RED, GREEN, YELLOW, BLUE, RED, BLUE, GREEN},
    //     .down = {BLUE, RED, GREEN, RED, YELLOW, BLUE, GREEN, BLUE, RED}
    // };

    Cube test_cube = {
        .up = {RED, GREEN, BLUE, BLUE, YELLOW, GREEN, GREEN, BLUE, RED},
        .left = {RED, BLUE, GREEN, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        .front = {GREEN, BLUE, RED, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        .right = {RED, GREEN, BLUE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        .back = {BLUE, GREEN, RED, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        .down = {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE}
    };

    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE U\n\n");
    cube_rotate_u(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE Z\n\n");
    cube_rotate_z(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE R\n\n");
    cube_rotate_r(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE Z\n\n");
    cube_rotate_z(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE D\n\n");
    cube_rotate_d(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE X\n\n");
    cube_rotate_x(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE F\n\n");
    cube_rotate_f(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE Y\n\n");
    cube_rotate_y(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE L\n\n");
    cube_rotate_l(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE Y\n\n");
    cube_rotate_y(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE B\n\n");
    cube_rotate_b(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);
}
