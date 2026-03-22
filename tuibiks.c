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
            face[(new_row * 3) + new_col] = temp[(row * 3) + col];
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
            face[(new_row * 3) + new_col] = temp[(row * 3) + col];
        }
    }
}

void cube_rotate_x(Cube *cube) {
    // Rotate the entire cube on R

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

    face_rotate_cw(cube->right);

    face_rotate_ccw(cube->left);
}

void cube_rotate_y(Cube *cube) {
    // Rotate the entire cube on U

    // B <- L <- F <- R <- B
    Color temp[9];
    memcpy(temp, cube->back, sizeof(temp));
    memcpy(cube->back, cube->left, sizeof(cube->back));
    memcpy(cube->left, cube->front, sizeof(cube->left));
    memcpy(cube->front, cube->right, sizeof(cube->front));
    memcpy(cube->right, temp, sizeof(cube->right));

    face_rotate_cw(cube->up);

    face_rotate_ccw(cube->down);
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
                colors[cube->up[(3 * row) + col]].r,
                colors[cube->up[(3 * row) + col]].g,
                colors[cube->up[(3 * row) + col]].b
            );
            printf("U ");
        }
        printf("\x1b[0m\n");
    }

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            printf(
                "\x1b[38;2;%d;%d;%dm",
                colors[cube->left[(3 * row) + col]].r,
                colors[cube->left[(3 * row) + col]].g,
                colors[cube->left[(3 * row) + col]].b
            );
            printf("L ");
        }
        for (int col = 0; col < 3; col++) {
            printf(
                "\x1b[38;2;%d;%d;%dm",
                colors[cube->front[(3 * row) + col]].r,
                colors[cube->front[(3 * row) + col]].g,
                colors[cube->front[(3 * row) + col]].b
            );
            printf("F ");
        }
        for (int col = 0; col < 3; col++) {
            printf(
                "\x1b[38;2;%d;%d;%dm",
                colors[cube->right[(3 * row) + col]].r,
                colors[cube->right[(3 * row) + col]].g,
                colors[cube->right[(3 * row) + col]].b
            );
            printf("R ");
        }
        for (int col = 0; col < 3; col++) {
            printf(
                "\x1b[38;2;%d;%d;%dm",
                colors[cube->back[(3 * row) + col]].r,
                colors[cube->back[(3 * row) + col]].g,
                colors[cube->back[(3 * row) + col]].b
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
                colors[cube->down[(3 * row) + col]].r,
                colors[cube->down[(3 * row) + col]].g,
                colors[cube->down[(3 * row) + col]].b
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
    Cube test_cube = {
        .up = {RED, GREEN, BLUE, BLUE, YELLOW, GREEN, GREEN, BLUE, RED},
        .left = {RED, BLUE, GREEN, BLUE, YELLOW, RED, GREEN, RED, BLUE},
        .front = {GREEN, BLUE, RED, RED, YELLOW, BLUE, BLUE, RED, GREEN},
        .right = {RED, GREEN, BLUE, BLUE, YELLOW, GREEN, GREEN, BLUE, RED},
        .back = {BLUE, GREEN, RED, GREEN, YELLOW, BLUE, RED, BLUE, GREEN},
        .down = {BLUE, RED, GREEN, RED, YELLOW, BLUE, GREEN, BLUE, RED}
    };

    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE X\n\n");
    cube_rotate_x(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE Y\n\n");
    cube_rotate_y(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);

    printf("\nROTATE Z\n\n");
    cube_rotate_z(&test_cube);
    render_cube(&test_cube);
    printf("\n");
    render_cube_net(&test_cube);
}
