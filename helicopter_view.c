/*---------------------------------------------------------------------------------
 * This file contains a program, drawing & animating a wind-mill.
 * This program is used to teach LCS and scene graph concepts.
 *
 * by S. K. Ueng, Nov.  2006
 */
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include "stb_image.h"

#define Step  0.5
#define  PI   3.1415926535
#define TSIZE 64 /* define texture dimension */

 /*-----Define a unit box--------*/
 /* Vertices of the box */
float  points[][3] = { {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5},
                      {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5},
                      {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5},
                      {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5} };
float  points_trapezoid[][3] = { {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5},
                      {0.3, 0.5, -0.1}, {-0.3, 0.5, -0.1},
                      {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5},
                      {0.3, 0.5, 0.5}, {-0.3, 0.5, 0.5} };//直角梯形立方
float  points_build[][3] = { {-0.4, -0.5, -0.4}, {0.4, -0.5, -0.4},
                      {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5},
                      {-0.4, -0.5, 0.4}, {0.4, -0.5, 0.4},
                      {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5} };//梯形立方
float  points_tilt[][3] = { {0.0, 3.0, -0.5}, {0.5, 3.0, -0.5},
                      {0.5, 3.5, -0.5}, {0.0, 3.5, -0.5},
                      {0.0, -0.5, 0.5}, {0.5, -0.5, 0.5},
                      {0.5, 0.0, 0.5}, {0.0, 0.0, 0.5} };//斜的桿子

float  points_triangle[][3] = { {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5},
                      {0.0, 0.5, 0.0}, {-0.5, -0.5, 0.5},
                      {0.5, -0.5, 0.5} };//三角錐
/* face of box, each face composing of 4 vertices */
int    face[][4] = { {1, 0, 3, 2}, {0, 1, 5, 4}, {5, 1, 2, 6},
                    {4, 5, 6, 7}, {7, 6, 2, 3}, {0, 4, 7, 3} };
float  normals[6][3] = { {0.0,0.0,-1.0}, {0.0,-1.0,0.0}, {1.0,0.0,0.0},
                     {0.0,0.0,1.0}, {0.0,1.0,0.0}, {-1.0,0.0,0.0} };
int    face_triangle[][3] = { {1, 0, 2}, {0, 3, 2}, {3, 4, 2}, {4, 1, 2} };
float  normals_triangle[4][3] = { {0.0,0.5,-1.0}, {-1.0,0.5,0.0}, {0.0,0.5,1.0}, {1.0,0.5,0.0}};
/* indices of the box faces */
int    cube[6] = { 0, 1, 2, 3, 4, 5 };

/*-----Define GLU quadric objects, a sphere and a cylinder----*/
GLUquadricObj* sphere = NULL, * cylind = NULL, * torus = NULL, * circle = NULL;

/*-----Define window size----*/
int width = 1200, height = 600;

int det = 1, bear_det = 1, bear_act = 0, bear_feet_det = 0;

//Global variables recording rotation angles of H-arm and blades.
float  swing_ang = 50.0, blade_ang = 0.0, bear_ang = 50.0, bear_ang_ver = 0.0, bear_act_move = 0.0, bear_feet_ang = -30.0, bear_ang_hoz = 0.0;

float  self_ang = 0.0, self_ang_ver = 0.0, self_ang_z = 0.0;
float  position[3] = { 8.0, 0.0, 0.0 };
int begin = 0;

float  firework_heigh = 0.0, firework_basis = 3.0;
int    firework_style = 0;//關煙火
int    firework_num = 5;//一圈的顆數

/*-----Translation and rotations of eye coordinate system---*/
float   eyeDx = 0.0, eyeDy = 0.0, eyeDz = 0.0;
float   eyeAngx = 0.0, eyeAngy = 0.0, eyeAngz = 0.0;
double  Eye[3] = { 0.0, 0.0, 30.0 }, Focus[3] = { 0.0, 0.0, 0.0 },
Vup[3] = { 0.0, 1.0, 0.0 };

float   u[3][3] = { {1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0} };
float   eye[3];
float   cv, sv; /* cos(5.0) and sin(5.0) */

float zoom = 0;//視角廣度
double windowclipping[6] = { -1.5,1.5,-1.5,1.5,1.5,40 };//視窗定義(near視窗left, near視窗right, near視窗bottom, near視窗top, near距離, far距離)
double ori_windowclipping[6] = { -1.5,1.5,-1.5,1.5,1.5,40 };

//helicopter視角
float   helicopter_eyeDx = 0.0, helicopter_eyeDy = 0.0, helicopter_eyeDz = 0.0;
float   helicopter_eyeAngx = 0.0, helicopter_eyeAngy = 0.0, helicopter_eyeAngz = 0.0;
double  helicopter_Eye[3] = { 13.0, 4.8, 4.0 }, helicopter_Focus[3] = { 0.0, 0.0, 0.0 },
helicopter_Vup[3] = { 0.0, 1.0, 0.0 };
float   helicopter_u[3][3] = { {0.0,0.0,-1.0}, {0.0,1.0,0.0}, {1.0,0.0,0.0} };//轉坐標系，z變原x，x變原-z
float   helicopter_eye[3];

/*-----Drawing stye
 0:4-windows, 1:x direction, 2:y direction, 3:z-dirtection, 4:perspective
 */
int style = 5;

/*----Define light source properties -------*/
float  sun_lit_position[] = { 70.0, 0.0, 30.0, 0.0 };//平行光，東昇西落的太陽
float  sun_lit_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
float  sun_lit_specular[] = { 0.1, 0.1, 0.1, 1.0 };
long long  sun_lit_angle = 0;

float  lamp_lit_position[] = { 37.5, 17.5, 41.5, 1.0 };//點光源，路燈
float  lamp_lit_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
float  lamp_lit_specular[] = { 1.0, 1.0, 0.8, 1.0 };
int    lamp_style = 1;//開
int    lamp_mod = 0;//常亮
int    lamp_mod1_act = 0;//現在要變弱，lamp_mod1_act = 1現在要變強
int    lamp_hold = 0;

float  search_lit_position[] = { 0.0, 0.0, 0.0, 1.0 };//spot light，探照燈
float  search_lit_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
float  search_lit_specular[] = { 1.0, 1.0, 0.0, 1.0 };
float  search_lit_cutoff = 30.0;
float  search_lit_direction[] = { -1.0, 0.0, 0.5, 0.0 };
float  search_lit_exponent = 8.0;
long long  search_lit_angle = 0;

float  firework_lit_position[] = { 70.0, 10.0, -20.0, 1.0 };//點光源，煙火
float  firework_lit_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
float  firework_lit_specular[] = { 0.0, 0.0, 1.0, 1.0 };

int    stage_on = 0;
float  stage1_lit_position[] = { 0.0, 0.0, 0.0, 1.0 };//spot light，舞台燈(紅)
float  stage1_lit_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
float  stage1_lit_specular[] = { 1.0, 0.0, 0.0, 1.0 };
float  stage1_lit_cutoff = 20.0;
float  stage1_lit_direction[] = { 0.0, 1.0, 3.732050807568877, 0.0 };
float  stage1_lit_exponent = 8.0;
long long  stage_lit_angle = 0;

float  stage2_lit_position[] = { 0.0, 0.0, 0.0, 1.0 };//spot light，舞台燈(綠)
float  stage2_lit_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
float  stage2_lit_specular[] = { 0.0, 1.0, 0.0, 1.0 };
float  stage2_lit_cutoff = 20.0;
float  stage2_lit_direction[] = { 0.0, 1.0, 3.732050807568877, 0.0 };
float  stage2_lit_exponent = 8.0;

float  stage3_lit_position[] = { 0.0, 0.0, 0.0, 1.0 };//spot light，舞台燈(藍)
float  stage3_lit_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
float  stage3_lit_specular[] = { 0.0, 0.0, 1.0, 1.0 };
float  stage3_lit_cutoff = 20.0;
float  stage3_lit_direction[] = { 0.0, 1.0, 3.732050807568877, 0.0 };
float  stage3_lit_exponent = 8.0;

float  global_ambient[] = { 0.2, 0.2, 0.2, 1.0 };

/*----Define material properties -----*/
float  helicopter_mat_ambient[] = { 0.329412, 0.223529, 0.027451, 1.0 };//黃銅
float  helicopter_mat_diffuse[] = { 0.780392, 0.568627, 0.113725, 1.0 };
float  helicopter_mat_specular[] = { 0.992157, 0.941176, 0.807843, 1.0 };
float  helicopter_mat_shininess = 27.897400;

float  search_mat_ambient[] = { 1.0, 1.0, 0.4392, 1.0 };
float  search_mat_diffuse[] = { 1.0, 1.0, 0.4392, 1.0 };
float  search_mat_specular[] = { 1.0, 1.0, 0.4392, 1.0 };
float  search_mat_emission[] = { 1.0, 1.0, 0.6588235, 1.0 };//自體發光
float  search_mat_shininess = 30.0;
float  search_color[] = { 1.0, 1.0, 0.4392 };

float  floor_mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
float  floor_mat_specular[] = { 0.0, 0.0, 0.0, 1.0 }; // 鏡面反射顏色，全零表示無鏡面反射
float  floor_mat_shininess = 0.0; //無光澤

float  castle_mat_ambient[] = { 0.110000, 0.060000, 0.090000, 1.0 };//紫羅蘭
float  castle_mat_diffuse[] = { 0.430000, 0.470000, 0.540000, 1.0 };
float  castle_mat_specular[] = { 0.330000, 0.330000, 0.520000, 1.0 };
float  castle_mat_shininess = 22.0;

float  build_mat_ambient[] = { 0.231250, 0.231250, 0.231250, 1.0 };//亮銀
float  build_mat_diffuse[] = { 0.277500, 0.277500, 0.277500, 1.0 };
float  build_mat_specular[] = { 0.773911, 0.773911, 0.773911, 1.0 };
float  build_mat_shininess = 89.599998;

float  leaf_mat_ambient[] = { 0.100000, 0.187250, 0.174500, 0.8 };//綠寶石
float  leaf_mat_diffuse[] = { 0.396000, 0.741510, 0.691020, 0.8 };
float  leaf_mat_specular[] = { 0.297254, 0.308290, 0.306678, 0.8 };
float  leaf_mat_shininess = 12.8;

float  bear_mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };//絨毛布料
float  bear_mat_diffuse[] = { 0.8, 0.6, 0.4, 1.0 };
float  bear_mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
float  bear_mat_shininess = 10.0;

float  chandelier_mat_ambient[] = { 0.229500, 0.088250, 0.027500, 1.0 };//亮銅
float  chandelier_mat_diffuse[] = { 0.550800, 0.211800, 0.066000, 1.0 };
float  chandelier_mat_specular[] = { 0.580594, 0.223257, 0.069570, 1.0 };
float  chandelier_mat_shininess = 51.200001;

float  firework_mat_ambient[] = { 0.0, 1.0, 1.0, 1.0 };
float  firework_mat_diffuse[] = { 0.741176, 1.0, 1.0, 1.0 };
float  firework_mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
float  firework_mat_emission[] = { 0.741176, 1.0, 1.0, 1.0 };
float  firework_mat_shininess = 1.0;

float  emission[] = { 0.0, 0.0, 0.0, 1.0 };

/*-----Create image space for textures -----*/
//unsigned char checkboard[TSIZE][TSIZE][4]; /* checkboard textures */
unsigned char brick[TSIZE][TSIZE][4];      /* brick wall textures */
unsigned int textName[12];                  /* declare two texture maps*/
float ds = 0.0, dt = 0.0;

//材質圖片
int image_width, image_height, nrChannels;
unsigned char* image_data;

/*---- the axes of billboard ----*/
float  a[3], b[3];
float    mtx[16];     /* the modelview matrix */
int padoru_num = 7;
int padoru_cnt = 0;
int padoru_mode = 0;
float padoru_angle = 0.0;

/*---Define fog color----*/
float fog_color[] = { 0.760784, 0.8745098, 1.0, 0.4 };
int fog_mode = 3;//0:關閉、1:GL_EXP、2:GL_EXP2、3:GL_LINEAR
float fog_density = 0.1;

void brick_pattern() {
    int y, x, i;

    for (y = 0; y < 12; y++) { /*----Create the first row of bricks----*/
        for (x = 0; x < 2; x++) {
            brick[y][x][0] = 191;
            brick[y][x][1] = 191;
            brick[y][x][2] = 191;
            brick[y][x][3] = 255;
        }

        for (x = 2; x < 30; x++) {
            brick[y][x][0] = 209;
            brick[y][x][1] = 232;
            brick[y][x][2] = 255;
            brick[y][x][3] = 255;
        }

        for (x = 30; x < 34; x++) {
            brick[y][x][0] = 191;
            brick[y][x][1] = 191;
            brick[y][x][2] = 191;
            brick[y][x][3] = 255;
        }

        for (x = 34; x < 62; x++) {
            brick[y][x][0] = 209;
            brick[y][x][1] = 232;
            brick[y][x][2] = 255;
            brick[y][x][3] = 255;
        }

        for (x = 62; x < 64; x++) {
            brick[y][x][0] = 191;
            brick[y][x][1] = 191;
            brick[y][x][2] = 191;
            brick[y][x][3] = 255;
        }
    }

    for (y = 12; y < 16; y++) /*--Create the cement between two breick layers */
        for (x = 0; x < 64; x++) {
            brick[y][x][0] = 191;
            brick[y][x][1] = 191;
            brick[y][x][2] = 191;
            brick[y][x][3] = 255;
        }

    for (y = 16; y < 28; y++) { /*--Create the 2nd layer of bricks---*/
        for (x = 0; x < 14; x++) {
            brick[y][x][0] = 209;
            brick[y][x][1] = 232;
            brick[y][x][2] = 255;
            brick[y][x][3] = 255;
        }

        for (x = 14; x < 18; x++) {
            brick[y][x][0] = 191;
            brick[y][x][1] = 191;
            brick[y][x][2] = 191;
            brick[y][x][3] = 255;
        }

        for (x = 18; x < 46; x++) {
            brick[y][x][0] = 209;
            brick[y][x][1] = 232;
            brick[y][x][2] = 255;
            brick[y][x][3] = 255;
        }

        for (x = 46; x < 50; x++) {
            brick[y][x][0] = 191;
            brick[y][x][1] = 191;
            brick[y][x][2] = 191;
            brick[y][x][3] = 255;
        }

        for (x = 50; x < 64; x++) {
            brick[y][x][0] = 209;
            brick[y][x][1] = 232;
            brick[y][x][2] = 255;
            brick[y][x][3] = 255;
        }
    }

    for (y = 28; y < 32; y++) /*---Add another layer of cement----*/
        for (x = 0; x < 64; x++) {
            brick[y][x][0] = 191;
            brick[y][x][1] = 191;
            brick[y][x][2] = 191;
            brick[y][x][3] = 255;
        }

    for (y = 32; y < 64; y++)
        for (x = 0; x < 64; x++)
            for (i = 0; i < 4; i++)
                brick[y][x][i] = brick[y - 32][x][i];
}

void texturemap() {
    /*-----Create texture ----*/
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //解壓縮像素數據的像素存儲模式
    glGenTextures(12, textName);//生成紋理名稱並將它們與數組textName關聯起來

    brick_pattern();
    glBindTexture(GL_TEXTURE_2D, textName[0]);//紋理與名稱為textName[0]的紋理對象綁定，以便後續的紋理操作將應用於該紋理

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, //Mipmap級別：指定要生成的紋理的細節級別，0表示基本級別
        GL_RGBA,
        TSIZE, TSIZE, //紋理寬、高
        0, //邊界大小
        GL_RGBA, GL_UNSIGNED_BYTE, brick);

    image_data = stbi_load("image\\badge.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    image_data = stbi_load("image\\stone_wall.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    image_data = stbi_load("image\\fluff.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    image_data = stbi_load("image\\motorcycle-1.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[4]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    image_data = stbi_load("image\\motorcycle-2.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[5]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    image_data = stbi_load("image\\barber.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[6]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    image_data = stbi_load("image\\padoru-0.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[7]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    image_data = stbi_load("image\\padoru-1.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[8]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    image_data = stbi_load("image\\padoru-2.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[9]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    image_data = stbi_load("image\\padoru-3.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[10]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    image_data = stbi_load("image\\sky.png", &image_width, &image_height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[11]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    //enable blending and alpha test
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void  myinit()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);      /*set the background color BLACK */
    /*Clear the Depth & Color Buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /*-----Set a parallel projection mode-----*/
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glOrtho(-15.0, 15.0, -15.0, 15.0, -10.5, 30.0);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);  /*Enable depth buffer for shading computing */
    glEnable(GL_NORMALIZE);   /*Enable mornalization  */
    /*---Create quadratic objects---*/
    if (sphere == NULL) {
        sphere = gluNewQuadric();
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
    }
    if (cylind == NULL) {
        cylind = gluNewQuadric();
        gluQuadricDrawStyle(cylind, GLU_FILL);
        gluQuadricNormals(cylind, GLU_SMOOTH);
    }
    if (torus == NULL) {
        torus = gluNewQuadric();
        gluQuadricDrawStyle(torus, GLU_FILL);
        gluQuadricNormals(torus, GLU_SMOOTH);
    }
    if (circle == NULL) {
        circle = gluNewQuadric();
        gluQuadricDrawStyle(circle, GLU_FILL);
        gluQuadricNormals(circle, GLU_SMOOTH);
    }
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricTexture(cylind, GL_TRUE);
    gluQuadricTexture(torus, GL_TRUE);
    gluQuadricTexture(circle, GL_TRUE);

    glEnable(GL_LIGHTING);    /*Enable lighting effects */
    glEnable(GL_LIGHT0);      /*Turn on light0 */
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    //glEnable(GL_LIGHT4);
    //glEnable(GL_LIGHT5);
    //glEnable(GL_LIGHT6);
    /*-----Define light0 ---------*/
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_lit_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_lit_specular);

    glLightfv(GL_LIGHT1, GL_POSITION, lamp_lit_position);

    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, search_lit_exponent);

    glLightfv(GL_LIGHT3, GL_DIFFUSE, firework_lit_diffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, firework_lit_specular);

    glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, stage1_lit_exponent);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, stage1_lit_diffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR, stage1_lit_specular);
    glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, stage1_lit_cutoff);

    glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, stage2_lit_exponent);
    glLightfv(GL_LIGHT5, GL_DIFFUSE, stage2_lit_diffuse);
    glLightfv(GL_LIGHT5, GL_SPECULAR, stage2_lit_specular);
    glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, stage2_lit_cutoff);

    glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, stage3_lit_exponent);
    glLightfv(GL_LIGHT6, GL_DIFFUSE, stage3_lit_diffuse);
    glLightfv(GL_LIGHT6, GL_SPECULAR, stage3_lit_specular);
    glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, stage3_lit_cutoff);

    /*-----Define some global lighting status -----*/
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); /* local viewer */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient); /*global ambient*/

    /*-----Enable face culling -----*/
    //glCullFace(GL_BACK); //剃除面，提高渲染性
    //glEnable(GL_CULL_FACE);

    glEnable(GL_COLOR_MATERIAL);//允許根據光照效果動態調整物體的顏色
    //決定對物體的正面還是反面，對環境光、鏡面光還是漫反射光進行顏色跟蹤
    //GL_AMBIENT_AND_DIFFUSE:顏色將同時用作環境光和漫反射的顏色

    /*---- Compute cos(5.0) and sin(5.0) ----*/
    cv = cos(5.0 * PI / 180.0);
    sv = sin(5.0 * PI / 180.0);
    /*---- Copy eye position ---*/
    eye[0] = Eye[0];
    eye[1] = Eye[1];
    eye[2] = Eye[2];

    helicopter_eye[0] = helicopter_Eye[0];
    helicopter_eye[1] = helicopter_Eye[1];
    helicopter_eye[2] = helicopter_Eye[2];

    zoom = 90;

    texturemap();

    /*----Set up fog conditions ---*/
    glEnable(GL_FOG);                /*enable fog fade */
    glFogi(GL_FOG_MODE, GL_LINEAR);  /*fog factor=GL_LINEAR,GL_EXP,or GL_EXP2*/
    glFogf(GL_FOG_DENSITY, 0.1);    /*fog opacity(density)= 0.15*/
    glFogf(GL_FOG_START, 1.0);       /*Setup two ends for GL_LINEAR*/
    glFogf(GL_FOG_END, 70.0);
    glFogfv(GL_FOG_COLOR, fog_color);/*set the fog color */

    //glFlush();/*Enforce window system display the results*/
}

/*-------------------------------------------------------
 * Procedure to compute the a[] and b[] axes of billboard
 * after eye parameters are specified.
 */
void compute_ab_axes(void) {
    float  w0, w2;
    double len;

    /*----Get w0 and w2 from the modelview matrix mtx[] ---*/
    w0 = mtx[2]; w2 = mtx[10];

    len = sqrt(w0 * w0 + w2 * w2);
    /*---- Define the a and b axes for billboards ----*/
    b[0] = 0.0; b[1] = 1.0; b[2] = 0.0;
    a[0] = w2 / len; a[1] = 0.0; a[2] = -w0 / len;
}

void draw_axes() {//畫世界坐標系軸
    glDisable(GL_LIGHTING);
    /*----Draw a white sphere to represent the original-----*/
    glPushMatrix();
    glColor3f(0.9, 0.9, 0.9);
    gluSphere(sphere, 2.0, 12, 12);

    /*----Draw three axes in colors, yellow, meginta, and cyan--*/
    /* Draw Z axis  */
    glColor3f(0.0, 0.95, 0.95);//cyan青
    gluCylinder(cylind, 0.5, 0.5, 10.0, 12, 3);

    /* Draw Y axis */
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glColor3f(0.95, 0.0, 0.95);//meginta紫
    gluCylinder(cylind, 0.5, 0.5, 10.0, 12, 3);
    glPopMatrix();

    /* Draw X axis */
    glColor3f(0.95, 0.95, 0.0);//yellow黃
    glPushMatrix();
    glRotatef(90.0, 0.0, 1.0, 0.0);
    gluCylinder(cylind, 0.5, 0.5, 10.0, 12, 3);
    glPopMatrix();
    /*-- Restore the original modelview matrix --*/
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void make_view(int x) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (x) {
    case 4:       /* Perspective 眼睛看到的*/
        /* In this sample program, eye position and Xe, Ye, Ze are computed
           by ourselves. Therefore, use them directly; no transform is
           applied upon eye coordinate system
           */
        gluLookAt(eye[0], eye[1], eye[2],
            eye[0] - u[2][0], eye[1] - u[2][1], eye[2] - u[2][2],
            u[1][0], u[1][1], u[1][2]);
        break;

    case 1:       /* X direction parallel viewing */
        gluLookAt(100.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //相機位置、面向的點、上方的軸(y)
        /* 看起來的軸
             y
             ^
             |
        z <--x
        */
        break;
    case 2:       /* Y direction parallel viewing */
        gluLookAt(0.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0); //相機位置、面向的點、上方的軸(x)
        /* 看起來的軸
        x
        ^
        |
        y--> z
        */
        break;
    case 3:       /* Z direction parallel viewing */
        gluLookAt(0.0, 0.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //相機位置、面向的點、上方的軸(y)
        /* 看起來的軸
        y
        ^
        |
        z--> x
        */
        break;
    case 5:       /* Perspective 直升機看到的*/
        gluLookAt(helicopter_eye[0], helicopter_eye[1], helicopter_eye[2],
            helicopter_eye[0] - helicopter_u[2][0], helicopter_eye[1] - helicopter_u[2][1], helicopter_eye[2] - helicopter_u[2][2],
            helicopter_u[1][0], helicopter_u[1][1], helicopter_u[1][2]);
        break;
    }

    glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
    compute_ab_axes();
}

void make_projection(int x) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (x == 4) {
        glFrustum(windowclipping[0], windowclipping[1], windowclipping[2],
            windowclipping[3], windowclipping[4], windowclipping[5]);
        //gluPerspective(90.0, (double)(width / 2) / (double)height, 1.5, 100.0);//設定透視投影矩陣
        //視野角度, 視窗的寬高比, near clipping plane距離視點的距離(視點之前的物體會被裁剪掉), far clipping plane距離視點的距離(視點之後的物體會被裁剪掉)
    }
    else if (x == 5) {
        gluPerspective(90.0, (double)(width / 2) / (double)height, 5, 300.0);//設定透視投影矩陣
    }
    else {
        if ((width / 2) > height)
            glOrtho(-40.0 * (zoom / 90), 40.0 * (zoom / 90), -40.0 * (zoom / 90) * (float)height / (float)(width / 2), 40.0 * (zoom / 90) * (float)height / (float)(width / 2),
                -0.0, 200.0);//設定正交投影矩陣
        /*left(裁剪區域的左邊界), right(裁剪區域的右邊界), bottom(裁剪區域的下邊界), top(裁剪區域的上邊界)
          , zNear(裁剪區域的近裁剪面距離), zFar(裁剪區域的遠裁剪面距離)*/
        else
            glOrtho(-40.0 * (zoom / 90) * (float)(width / 2) / (float)height, 40.0 * (zoom / 90) * (float)(width / 2) / (float)height, -40.0 * (zoom / 90), 40.0 * (zoom / 90),
                -0.0, 200.0);
    }
    glMatrixMode(GL_MODELVIEW);
}

void draw_viewvolume() {
    glDisable(GL_LIGHTING);
    double wcn = fabs(windowclipping[1] - windowclipping[0]) / 2;//weight_center_near
    double hcn = fabs(windowclipping[3] - windowclipping[2]) / 2;//high_center_near
    double dcn = windowclipping[4];//deep_center_near
    double wcf = (windowclipping[5] * wcn / windowclipping[4]);//weight_center_far
    double hcf = (windowclipping[5] * hcn / windowclipping[4]);//high_center_far
    double dcf = windowclipping[5];//deep_center_far

    glColor4f(0.9, 0.9, 0.9, 0.1);
    glBegin(GL_TRIANGLES);//畫眼睛到near_clip_plane的四個三角形
    //上面那片
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] - dcn * u[2][0] + hcn * u[1][0] + wcn * u[0][0]
        , eye[1] - dcn * u[2][1] + hcn * u[1][1] + wcn * u[0][1]
        , eye[2] - dcn * u[2][2] + hcn * u[1][2] + wcn * u[0][2]);//near右上
    glVertex3f(eye[0] - dcn * u[2][0] + hcn * u[1][0] - wcn * u[0][0]
        , eye[1] - dcn * u[2][1] + hcn * u[1][1] - wcn * u[0][1]
        , eye[2] - dcn * u[2][2] + hcn * u[1][2] - wcn * u[0][2]);//near左上

    //左邊那片
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] - dcn * u[2][0] + hcn * u[1][0] - wcn * u[0][0]
        , eye[1] - dcn * u[2][1] + hcn * u[1][1] - wcn * u[0][1]
        , eye[2] - dcn * u[2][2] + hcn * u[1][2] - wcn * u[0][2]);//near左上
    glVertex3f(eye[0] - dcn * u[2][0] - hcn * u[1][0] - wcn * u[0][0]
        , eye[1] - dcn * u[2][1] - hcn * u[1][1] - wcn * u[0][1]
        , eye[2] - dcn * u[2][2] - hcn * u[1][2] - wcn * u[0][2]);//near左下

    //下面那片
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] - dcn * u[2][0] - hcn * u[1][0] - wcn * u[0][0]
        , eye[1] - dcn * u[2][1] - hcn * u[1][1] - wcn * u[0][1]
        , eye[2] - dcn * u[2][2] - hcn * u[1][2] - wcn * u[0][2]);//near左下
    glVertex3f(eye[0] - dcn * u[2][0] - hcn * u[1][0] + wcn * u[0][0]
        , eye[1] - dcn * u[2][1] - hcn * u[1][1] + wcn * u[0][1]
        , eye[2] - dcn * u[2][2] - hcn * u[1][2] + wcn * u[0][2]);//near右下

    //右邊那片
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] - dcn * u[2][0] - hcn * u[1][0] + wcn * u[0][0]
        , eye[1] - dcn * u[2][1] - hcn * u[1][1] + wcn * u[0][1]
        , eye[2] - dcn * u[2][2] - hcn * u[1][2] + wcn * u[0][2]);//near右下
    glVertex3f(eye[0] - dcn * u[2][0] + hcn * u[1][0] + wcn * u[0][0]
        , eye[1] - dcn * u[2][1] + hcn * u[1][1] + wcn * u[0][1]
        , eye[2] - dcn * u[2][2] + hcn * u[1][2] + wcn * u[0][2]);//near右上
    glEnd();

    glColor4f(0.9, 0.9, 0.9, 0.7);
    glBegin(GL_QUADS);
    //上面那片
    glVertex3f(eye[0] - dcn * u[2][0] + hcn * u[1][0] + wcn * u[0][0]
        , eye[1] - dcn * u[2][1] + hcn * u[1][1] + wcn * u[0][1]
        , eye[2] - dcn * u[2][2] + hcn * u[1][2] + wcn * u[0][2]);//near右上
    glVertex3f(eye[0] - dcf * u[2][0] + hcf * u[1][0] + wcf * u[0][0]
        , eye[1] - dcf * u[2][1] + hcf * u[1][1] + wcf * u[0][1]
        , eye[2] - dcf * u[2][2] + hcf * u[1][2] + wcf * u[0][2]);//far右上
    glVertex3f(eye[0] - dcf * u[2][0] + hcf * u[1][0] - wcf * u[0][0]
        , eye[1] - dcf * u[2][1] + hcf * u[1][1] - wcf * u[0][1]
        , eye[2] - dcf * u[2][2] + hcf * u[1][2] - wcf * u[0][2]);//far左上
    glVertex3f(eye[0] - dcn * u[2][0] + hcn * u[1][0] - wcn * u[0][0]
        , eye[1] - dcn * u[2][1] + hcn * u[1][1] - wcn * u[0][1]
        , eye[2] - dcn * u[2][2] + hcn * u[1][2] - wcn * u[0][2]);//near左上

    //左邊那片
    glVertex3f(eye[0] - dcn * u[2][0] + hcn * u[1][0] - wcn * u[0][0]
        , eye[1] - dcn * u[2][1] + hcn * u[1][1] - wcn * u[0][1]
        , eye[2] - dcn * u[2][2] + hcn * u[1][2] - wcn * u[0][2]);//near左上
    glVertex3f(eye[0] - dcf * u[2][0] + hcf * u[1][0] - wcf * u[0][0]
        , eye[1] - dcf * u[2][1] + hcf * u[1][1] - wcf * u[0][1]
        , eye[2] - dcf * u[2][2] + hcf * u[1][2] - wcf * u[0][2]);//far左上
    glVertex3f(eye[0] - dcf * u[2][0] - hcf * u[1][0] - wcf * u[0][0]
        , eye[1] - dcf * u[2][1] - hcf * u[1][1] - wcf * u[0][1]
        , eye[2] - dcf * u[2][2] - hcf * u[1][2] - wcf * u[0][2]);//far左下
    glVertex3f(eye[0] - dcn * u[2][0] - hcn * u[1][0] - wcn * u[0][0]
        , eye[1] - dcn * u[2][1] - hcn * u[1][1] - wcn * u[0][1]
        , eye[2] - dcn * u[2][2] - hcn * u[1][2] - wcn * u[0][2]);//near左下

    //下面那片
    glVertex3f(eye[0] - dcn * u[2][0] - hcn * u[1][0] - wcn * u[0][0]
        , eye[1] - dcn * u[2][1] - hcn * u[1][1] - wcn * u[0][1]
        , eye[2] - dcn * u[2][2] - hcn * u[1][2] - wcn * u[0][2]);//near左下
    glVertex3f(eye[0] - dcf * u[2][0] - hcf * u[1][0] - wcf * u[0][0]
        , eye[1] - dcf * u[2][1] - hcf * u[1][1] - wcf * u[0][1]
        , eye[2] - dcf * u[2][2] - hcf * u[1][2] - wcf * u[0][2]);//far左下
    glVertex3f(eye[0] - dcf * u[2][0] - hcf * u[1][0] + wcf * u[0][0]
        , eye[1] - dcf * u[2][1] - hcf * u[1][1] + wcf * u[0][1]
        , eye[2] - dcf * u[2][2] - hcf * u[1][2] + wcf * u[0][2]);//far右下
    glVertex3f(eye[0] - dcn * u[2][0] - hcn * u[1][0] + wcn * u[0][0]
        , eye[1] - dcn * u[2][1] - hcn * u[1][1] + wcn * u[0][1]
        , eye[2] - dcn * u[2][2] - hcn * u[1][2] + wcn * u[0][2]);//near右下

    //右邊那片
    glVertex3f(eye[0] - dcn * u[2][0] - hcn * u[1][0] + wcn * u[0][0]
        , eye[1] - dcn * u[2][1] - hcn * u[1][1] + wcn * u[0][1]
        , eye[2] - dcn * u[2][2] - hcn * u[1][2] + wcn * u[0][2]);//near右下
    glVertex3f(eye[0] - dcf * u[2][0] - hcf * u[1][0] + wcf * u[0][0]
        , eye[1] - dcf * u[2][1] - hcf * u[1][1] + wcf * u[0][1]
        , eye[2] - dcf * u[2][2] - hcf * u[1][2] + wcf * u[0][2]);//far右下
    glVertex3f(eye[0] - dcf * u[2][0] + hcf * u[1][0] + wcf * u[0][0]
        , eye[1] - dcf * u[2][1] + hcf * u[1][1] + wcf * u[0][1]
        , eye[2] - dcf * u[2][2] + hcf * u[1][2] + wcf * u[0][2]);//far右上
    glVertex3f(eye[0] - dcn * u[2][0] + hcn * u[1][0] + wcn * u[0][0]
        , eye[1] - dcn * u[2][1] + hcn * u[1][1] + wcn * u[0][1]
        , eye[2] - dcn * u[2][2] + hcn * u[1][2] + wcn * u[0][2]);//near右上
    glEnd();
    glEnable(GL_LIGHTING);
}

void draw_view() {//畫眼睛坐標系軸
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_LIGHTING);
    /*----Draw Eye position-----*/
    glPushMatrix();
    glTranslatef(eye[0], eye[1], eye[2]);
    glColor3f(0.0, 1.0, 0.0);
    glutWireSphere(1.0, 10, 10);
    glPopMatrix();

    /*----Draw eye coord. axes -----*/
    glColor3f(1.0, 1.0, 0.0);           /* Draw Xe */
    glBegin(GL_LINES);
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] + 20.0 * u[0][0], eye[1] + 20.0 * u[0][1], eye[2] + 20.0 * u[0][2]);
    glEnd();

    glColor3f(1.0, 0.0, 1.0);          /* Draw Ye */
    glBegin(GL_LINES);
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] + 20.0 * u[1][0], eye[1] + 20.0 * u[1][1], eye[2] + 20.0 * u[1][2]);
    glEnd();

    glColor3f(0.0, 1.0, 1.0);          /* Draw Ze */
    glBegin(GL_LINES);
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] + 20.0 * u[2][0], eye[1] + 20.0 * u[2][1], eye[2] + 20.0 * u[2][2]);
    glEnd();
    glEnable(GL_LIGHTING);
}

void draw_view_hel() {//畫直升機視角坐標系軸
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_LIGHTING);
    /*----Draw Eye position-----*/
    glPushMatrix();
    glTranslatef(helicopter_eye[0], helicopter_eye[1], helicopter_eye[2]);
    glColor3f(0.0, 1.0, 0.0);
    glutWireSphere(1.0, 10, 10);
    glPopMatrix();

    /*----Draw eye coord. axes -----*/
    glColor3f(1.0, 1.0, 0.0);           /* Draw Xe */
    glBegin(GL_LINES);
    glVertex3f(helicopter_eye[0], helicopter_eye[1], helicopter_eye[2]);
    glVertex3f(helicopter_eye[0] + 20.0 * helicopter_u[0][0], helicopter_eye[1] + 20.0 * helicopter_u[0][1], helicopter_eye[2] + 20.0 * helicopter_u[0][2]);
    glEnd();

    glColor3f(1.0, 0.0, 1.0);          /* Draw Ye */
    glBegin(GL_LINES);
    glVertex3f(helicopter_eye[0], helicopter_eye[1], helicopter_eye[2]);
    glVertex3f(helicopter_eye[0] + 20.0 * helicopter_u[1][0], helicopter_eye[1] + 20.0 * helicopter_u[1][1], helicopter_eye[2] + 20.0 * helicopter_u[1][2]);
    glEnd();

    glColor3f(0.0, 1.0, 1.0);          /* Draw Ze */
    glBegin(GL_LINES);
    glVertex3f(helicopter_eye[0], helicopter_eye[1], helicopter_eye[2]);
    glVertex3f(helicopter_eye[0] + 20.0 * helicopter_u[2][0], helicopter_eye[1] + 20.0 * helicopter_u[2][1], helicopter_eye[2] + 20.0 * helicopter_u[2][2]);
    glEnd();
    glEnable(GL_LIGHTING);
}

/*-------------------------------------------------------
 * Procedure to draw a polygon as a blade of the windmill
 */
void draw_blade()
{
    glNormal3f(0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.0, 0.2, 0.0);
    glVertex3f(0.2, 1.0, 0.0);
    glVertex3f(0.2, 2.0, 0.0);
    glVertex3f(-0.2, 2.0, 0.0);
    glVertex3f(-0.2, 1.0, 0.0);
    glEnd();
}

/*--------------------------------------------------------
 * Procedure to draw a cube. The geometrical data of the cube
 * are defined above.
 */
void draw_cube()
{
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    int    i;
    for (i = 0; i < 6; i++) {
        glNormal3fv(normals[i]);
        glBegin(GL_POLYGON);  /* Draw the face */
        glTexCoord2f(1.0, 1.0); glVertex3fv(points[face[i][0]]);
        glTexCoord2f(0.0, 1.0); glVertex3fv(points[face[i][1]]);
        glTexCoord2f(0.0, 0.0); glVertex3fv(points[face[i][2]]);
        glTexCoord2f(1.0, 0.0); glVertex3fv(points[face[i][3]]);
        glEnd();
    }
}

void draw_trapezoid()
{
    int    i;

    glColor3f(0.20, 0.75, 0.0);  /* Set the current color */
    for (i = 0; i < 6; i++) {
        glNormal3fv(normals[i]);
        glBegin(GL_POLYGON);  /* Draw the face */
        glVertex3fv(points_trapezoid[face[i][0]]);
        glVertex3fv(points_trapezoid[face[i][1]]);
        glVertex3fv(points_trapezoid[face[i][2]]);
        glVertex3fv(points_trapezoid[face[i][3]]);
        glEnd();
    }
}

void draw_tilt()
{
    int    i;
    for (i = 0; i < 6; i++) {
        glNormal3fv(normals[i]);
        glBegin(GL_POLYGON);  /* Draw the face */
        glVertex3fv(points_tilt[face[i][0]]);
        glVertex3fv(points_tilt[face[i][1]]);
        glVertex3fv(points_tilt[face[i][2]]);
        glVertex3fv(points_tilt[face[i][3]]);
        glEnd();
    }
}

void draw_triangle() {
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(ds, dt, 0.0);
    glMatrixMode(GL_MODELVIEW);

    for (int i = 0; i < 4; i++) {
        glNormal3fv(normals_triangle);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3fv(points_triangle[face_triangle[i][0]]);
        glTexCoord2f(1.0, 0.0); glVertex3fv(points_triangle[face_triangle[i][1]]);
        glTexCoord2f(0.5, 1.0); glVertex3fv(points_triangle[face_triangle[i][2]]);
        glEnd();
    }
    glNormal3f(0.0, -1.0, 0.0);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3fv(points_triangle[0]);
    glTexCoord2f(1.0, 0.0); glVertex3fv(points_triangle[1]);
    glTexCoord2f(1.0, 1.0); glVertex3fv(points_triangle[4]);
    glTexCoord2f(0.0, 1.0); glVertex3fv(points_triangle[3]);
    glEnd();
}

void draw_rectangle(int front_back) {
    if (front_back == 0) glNormal3f(0.0, 0.0, 1.0);
    else glNormal3f(0.0, 0.0, -1.0);
    glBegin(GL_POLYGON);  /* Draw the face */
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, 1.0, 0.0);
    glEnd();
}

void draw_build()
{
    int    i;
    
    for (i = 0; i < 6; i++) {
        glNormal3fv(normals[i]);
        glBegin(GL_POLYGON);  /* Draw the face */
        glVertex3fv(points_build[face[i][0]]);
        glVertex3fv(points_build[face[i][1]]);
        glVertex3fv(points_build[face[i][2]]);
        glVertex3fv(points_build[face[i][3]]);
        glEnd();
    }
}

void draw_floor()
{
    int  i, j;
    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floor_mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, floor_mat_shininess);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);//在啟動光照系統之後，為圖元指定顏色變得不太方便

    glNormal3f(0.0, 1.0, 0.0);
    for (i = 0; i < 15; i++) {
        for (j = -5; j < 15; j++) {
            if ((i + j) % 2 == 0) glColor3f(1.0, 1.0, 1.0);
            else glColor3f(1.0, 0.7, 0.7);
            glBegin(GL_POLYGON);
            glVertex3f((i - 5.0) * 10.0, -0.01, (j - 5.0) * 10.0);
            glVertex3f((i - 5.0) * 10.0, -0.01, (j - 4.0) * 10.0);
            glVertex3f((i - 4.0) * 10.0, -0.01, (j - 4.0) * 10.0);
            glVertex3f((i - 4.0) * 10.0, -0.01, (j - 5.0) * 10.0);
            glEnd();
        }
    }
    glPopMatrix();
}

void heigh_build() {//大樓
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, build_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, build_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, build_mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, build_mat_shininess);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

        glColor3f(0.31, 0.31, 0.31);
        glTranslatef(-20.0, 4.5, 0.0);
        glPushMatrix();
            glScaled(9.0, 9.0, 9.0);
            draw_build();
            glTranslatef(0.0, 1.0, 0.0);
            draw_build();
            glTranslatef(0.0, 1.0, 0.0);
            draw_build();
            glTranslatef(0.0, 1.0, 0.0);
            draw_build();
        glPopMatrix();

        glTranslatef(0.0, 34, 0.0);
        glScaled(1.5, 5.0, 1.5);
        draw_cube();
    glPopMatrix();
}

void draw_tree() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, leaf_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leaf_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, leaf_mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, leaf_mat_shininess);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glPushMatrix();
        glTranslatef(10.0, -10.0, 0.0);

        glPushMatrix();
            glColor3f(0.52, 0.17, 0.0);
            glScaled(1.0, 1.0, 10.0);
            gluCylinder(cylind, 1.0, 1.0, 2.0, 12, 3);
        glPopMatrix();

        glPushMatrix();
            glColor3f(0.0, 0.92, 0.0);
            glTranslatef(0.0, 0.0, 20.0);
            glScaled(6.0, 6.0, 7.0);
            gluSphere(sphere, 1.0, 12, 12);
        glPopMatrix();

        glPushMatrix();
            glColor3f(0.52, 0.17, 0.0);
            glTranslatef(0.0, 0.0, 5.0);
            glRotatef(-90, 0.0, 0.0, 1.0);
            glRotatef(45, 1.0, 0.0, 0.0);
            glPushMatrix();
                glScaled(0.5, 0.5, 3.0);
                gluCylinder(cylind, 1.0, 1.0, 2.0, 12, 3);
            glPopMatrix();
            glColor3f(0.0, 0.92, 0.0);
            glTranslatef(0.0, 0.0, 7.0);
            glScaled(2.0, 2.0, 3.0);
            gluSphere(sphere, 1.0, 12, 12);
        glPopMatrix();

        glPushMatrix();
            glColor3f(0.52, 0.17, 0.0);
            glTranslatef(0.0, 0.0, 7.0);
            glRotatef(-90, 0.0, 0.0, 1.0);
            glRotatef(-45, 1.0, 0.0, 0.0);
            glPushMatrix();
                glScaled(0.5, 0.5, 3.0);
                gluCylinder(cylind, 1.0, 1.0, 2.0, 12, 3);
            glPopMatrix();
            glColor3f(0.0, 0.92, 0.0);
            glTranslatef(0.0, 0.0, 7.0);
            glScaled(2.0, 2.0, 3.0);
            gluSphere(sphere, 1.0, 12, 12);
        glPopMatrix();

    glPopMatrix();
}

void draw_chandelier() {
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, chandelier_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, chandelier_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, chandelier_mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, chandelier_mat_shininess);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

        glColor3f(0.45098, 0.22745, 0.22745);
        glTranslatef(0.0, 13.0, 0.0);
        glPushMatrix();
            glScaled(1.5, 26.0, 1.5);
            draw_cube();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(5.0, 13.5, 0.0);
            glScaled(15.0, 1.5, 1.5);
            draw_cube();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(10.0, 12.0, 0.0);
            gluSphere(sphere, 1.0, 12, 12);
            glTranslatef(0.0, -3.5, 0.0);
            glScaled(1.0, 6.0, 1.0);
            draw_cube();
        glPopMatrix();
        glPushMatrix();
            glDisable(GL_ALPHA_TEST);
            glColor4f(1.0, 1.0, 0.8, 0.5);
            glTranslatef(10.0, 4.6, 0.0);
            glScaled(2.0, 2.0, 2.0);
            glDisable(GL_LIGHTING);
            draw_build();
            glEnable(GL_LIGHTING);
            glEnable(GL_ALPHA_TEST);
        glPopMatrix();
    glPopMatrix();
}

void draw_stagelight() {
    glPushMatrix();
        glRotatef(90, 1.0, 0.0, 0.0);
        glColor3f(0.1490196, 0.1490196, 0.1490196);
        glTranslatef(0.0, 2.0, -20.0);
        glRotatef(120.0, 1.0, 0.0, 0.0);
        gluCylinder(cylind, 1.0, 0.1, 3.0, 12, 6);

        glRotatef(-120.0, 1.0, 0.0, 0.0);//回到中心
        glTranslatef(0.0, -2.0, 20.0);//回到中心
        glRotatef(120.0, 0.0, 1.0, 0.0);
        glTranslatef(0.0, 2.0, -20.0);
        glRotatef(120.0, 1.0, 0.0, 0.0);
        gluCylinder(cylind, 1.0, 0.1, 3.0, 12, 6);
        glRotatef(-120.0, 1.0, 0.0, 0.0);//回到中心
        glTranslatef(0.0, -2.0, 20.0);//回到中心
        glRotatef(120.0, 0.0, 1.0, 0.0);
        glTranslatef(0.0, 2.0, -20.0);
        glRotatef(120.0, 1.0, 0.0, 0.0);
        gluCylinder(cylind, 1.0, 0.1, 3.0, 12, 6);
    glPopMatrix();
}

void draw_park() {
    glPushMatrix();
    //公園
        glTranslatef(40.0, 0.01, 50.0);//高一點才不會跟地板重疊
        glRotatef(-90, 1.0, 0.0, 0.0);

        //草皮
        glMaterialfv(GL_FRONT, GL_AMBIENT, leaf_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, leaf_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, leaf_mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, leaf_mat_shininess);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

        glPushMatrix();
            glScaled(20.0, 20.0, 20.0);
            glColor3f(0.0, 0.74, 0.0);
            gluDisk(circle, 0, 1, 16, 3);
        glPopMatrix();

        glMaterialfv(GL_FRONT, GL_AMBIENT, build_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, build_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, build_mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, build_mat_shininess);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        draw_stagelight();
        
        //鞦韆
        glPushMatrix();
            glColor3f(0.68, 0.68, 0.68);
            glTranslatef(-10.0, 5.0, 5.0);
            glScaled(1.0, 1.0, 10.0);
            draw_tilt();
            glScaled(1.0, -1.0, 1.0);
            glTranslatef(0.0, 1.5, 0.0);
            draw_tilt();
            glScaled(1.0, -1.0, 1.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0, 5.0, 5.0);
            glScaled(1.0, 1.0, 10.0);
            draw_tilt();
            glScaled(1.0, -1.0, 1.0);
            glTranslatef(0.0, 1.5, 0.0);
            draw_tilt();
            glScaled(1.0, -1.0, 1.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-5.0, 4.25, 10,0);
            glPushMatrix();
                glScaled(10.0, 0.5, 0.5);
                draw_cube();
            glPopMatrix();

            glPushMatrix();
                glRotatef(swing_ang, 1.0, 0.0, 0.0);
                glPushMatrix();
                    glTranslatef(-2.5, 0.0, -0.5);
                    glScaled(1.5, 1.5, 1.5);
                    gluSphere(sphere, 0.25, 12, 12);
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(-2.5, 0.0, -4.0);
                    glScaled(0.5, 0.5, 7.0);
                    draw_cube();
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(2.5, 0.0, -0.5);
                    glScaled(1.5, 1.5, 1.5);
                    gluSphere(sphere, 0.25, 12, 12);
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(2.5, 0.0, -4.0);
                    glScaled(0.5, 0.5, 7.0);
                    draw_cube();
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(0.0, 0.0, -7.3);
                    glScaled(5.5, 3.0, 0.6);
                    draw_cube();
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();

        glMaterialfv(GL_FRONT, GL_AMBIENT, leaf_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, leaf_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, leaf_mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, leaf_mat_shininess);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        draw_tree();

        glPushMatrix();
            glRotatef(90, 1.0, 0.0, 0.0);
            glTranslatef(-10.0, 0.0, -15.0);
            glRotatef(-45.0, 0.0, 1.0, 0.0);
            draw_chandelier();
        glPopMatrix();

    glPopMatrix();
}

void draw_bear() {
    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindTexture(GL_TEXTURE_2D, textName[3]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_REPLACE
    
        glMaterialfv(GL_FRONT, GL_AMBIENT, bear_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, bear_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, bear_mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, bear_mat_shininess);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

        glTranslatef(5.0, 3.5, 70.0);
        glRotatef(bear_ang_hoz, 0.0, 1.0, 0.0);
        glRotatef(bear_ang, 1.0, 0.0, 0.0);
        glTranslatef(bear_act_move, 0.0, 0.0);
        glRotatef(bear_ang_ver, 0.0, 0.0, 1.0);
        glTranslatef(-bear_act_move, 0.0, 0.0);

        glPushMatrix();//身體
            glColor3f(0.82, 0.41, 0.0);
            glScaled(4.4, 3.5, 4.0);
            gluSphere(sphere, 1.0, 24, 12);
        glPopMatrix();

        glPushMatrix();//上半
            glTranslatef(-5.0, 0.0, 0.0);

            glPushMatrix();//頭
                glColor3f(0.82, 0.41, 0.0);
                glScaled(2.5, 2.5, 3.0);
                gluSphere(sphere, 1.0, 24, 12);
            glPopMatrix();

            glPushMatrix();//左眼睛
                glColor3f(0.0, 0.0, 0.0);
                glTranslatef(-1.0, 2.0, 1.1);
                glScaled(0.35, 0.35, 0.35);
                gluSphere(sphere, 1.0, 12, 12);
            glPopMatrix();
            glPushMatrix();//右眼睛
                glColor3f(0.0, 0.0, 0.0);
                glTranslatef(-1.0, 2.0, -1.1);
                glScaled(0.35, 0.35, 0.35);
                gluSphere(sphere, 1.0, 12, 12);
            glPopMatrix();

            glPushMatrix();//左耳朵
                glColor3f(0.52, 0.26, 0.0);
                glTranslatef(-2.5, 0.3, 1.8);
                glRotatef(20.0, 0.0, 1.0, 0.0);
                glScaled(0.8, 0.8, 1.0);
                gluSphere(sphere, 1.0, 12, 12);
            glPopMatrix();
            glPushMatrix();//右耳朵
                glColor3f(0.52, 0.26, 0.0);
                glTranslatef(-2.5, 0.3, -1.8);
                glRotatef(-20.0, 0.0, 1.0, 0.0);
                glScaled(0.8, 0.8, 1.0);
                gluSphere(sphere, 1.0, 12, 12);
            glPopMatrix();
        glPopMatrix();

        glPushMatrix();//下半
            glPushMatrix();//左手
                glColor3f(0.52, 0.26, 0.0);
                glTranslatef(-3.0, 1.0, 3.5);
                glRotatef(30.0, 0.0, 0.0, 1.0);
                glRotatef(45.0, 1.0, 0.0, 0.0);
                glScaled(0.8, 2.0, 0.8);
                gluSphere(sphere, 1.0, 15, 15);
            glPopMatrix();
            glPushMatrix();//右手
                glColor3f(0.52, 0.26, 0.0);
                glTranslatef(-3.0, 1.0, -3.5);
                glRotatef(30.0, 0.0, 0.0, 1.0);
                glRotatef(-45.0, 1.0, 0.0, 0.0);
                glScaled(0.8, 2.0, 0.8);
                gluSphere(sphere, 1.0, 15, 15);
            glPopMatrix();
            glPushMatrix();//左腳
                glColor3f(0.52, 0.26, 0.0);
                glTranslatef(2.6, 2.0, 3.0);
                glTranslatef(0.0, -1.0, 0.0);
                glRotatef(40.0, 1.0, 0.0, 0.0);
                glRotatef(bear_feet_ang, 0.0, 0.0, 1.0);
                glTranslatef(0.0, 1.0, 0.0);
                glScaled(0.8, 2.0, 0.8);
                gluSphere(sphere, 1.0, 15, 15);
            glPopMatrix();
            glPushMatrix();//右腳
                glColor3f(0.52, 0.26, 0.0);
                glTranslatef(2.6, 2.0, -3.0);
                glTranslatef(0.0, -1.0, 0.0);
                glRotatef(-40.0, 1.0, 0.0, 0.0);
                glRotatef(bear_feet_ang, 0.0, 0.0, 1.0);
                glTranslatef(0.0, 1.0, 0.0);
                glScaled(0.8, 2.0, 0.8);
                gluSphere(sphere, 1.0, 15, 15);
            glPopMatrix();
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void draw_banner() {
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glNormal3f(0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);  /* Draw the face */
    glTexCoord2f(0.0, 1.0); glVertex3f(0.0, -0.5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.5, -0.5, 0.0);
    glTexCoord2f(0.66, 0.5); glVertex3f(1.0, 0.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.5, 0.5, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.5, 0.0);
    glEnd();
}

void draw_tower() {
    glPushMatrix();//右塔
        glTranslatef(16.0, -8.0, 0.0);
        glPushMatrix();//右塔身
            glColor3f(0.678, 0.84, 1.0);
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glScaled(11.0, 11.0, 30.0);
            gluCylinder(cylind, 0.5, 0.5, 1.0, 24, 3);
        glPopMatrix();
        glPushMatrix();//右塔屋頂
            glEnable(GL_TEXTURE_2D);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBindTexture(GL_TEXTURE_2D, textName[6]);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            glTranslatef(ds, dt, 0.0);
            glMatrixMode(GL_MODELVIEW);
            glColor3f(1.0, 1.0, 1.0);
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glTranslatef(0.0, 0.0, 30.0);
            //glColor3f(0.4, 0.7, 1.0);
            glScaled(14.0, 14.0, 10.0);
            gluCylinder(cylind, 0.5, 0.01, 1.0, 24, 3);
            glDisable(GL_TEXTURE_2D);
        glPopMatrix();
        glPushMatrix();//窗戶們
            glColor3f(0.988, 0.988, 0.0);
            glTranslatef(0.0, 5.0, 5.5);
            glScaled(1.0, 5.0, 1.0);
            draw_rectangle(0);
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.988, 0.988, 0.0);
            glRotatef(180.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 8.0, 5.5);
            glScaled(1.0, 5.0, 1.0);
            draw_rectangle(0);
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.988, 0.988, 0.0);
            glRotatef(80.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 3.0, 5.5);
            glScaled(1.0, 5.0, 1.0);
            draw_rectangle(0);
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.988, 0.988, 0.0);
            glRotatef(45.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 20.0, 5.5);
            glScaled(1.0, 5.0, 1.0);
            draw_rectangle(0);
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.988, 0.988, 0.0);
            glRotatef(135.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 23.0, 5.5);
            glScaled(1.0, 5.0, 1.0);
            draw_rectangle(0);
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.988, 0.988, 0.0);
            glRotatef(-30.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 15.0, 5.5);
            glScaled(1.0, 5.0, 1.0);
            draw_rectangle(0);
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.988, 0.988, 0.0);
            glRotatef(120.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 13.0, 5.5);
            glScaled(1.0, 5.0, 1.0);
            draw_rectangle(0);
        glPopMatrix();
    glPopMatrix();
}

void draw_castle() {
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, castle_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, castle_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, castle_mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, castle_mat_shininess);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glTranslatef(5.0, 8.0, -40.0);

        glPushMatrix();//主體
            glEnable(GL_TEXTURE_2D);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBindTexture(GL_TEXTURE_2D, textName[2]);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_REPLACE
            glColor3f(0.82, 0.91, 1.0);
            glScaled(21.0, 16.0, 21.0);
            draw_cube();
            glDisable(GL_TEXTURE_2D);
        glPopMatrix();
        glPushMatrix();//大門
            glTranslatef(-4.0, -8.0, 10.501);
            glPushMatrix();
                glColor3f(0.988, 0.988, 0.0);
                glScaled(8.0, 9.0, 1.0);
                draw_rectangle(0);
            glPopMatrix();
            glPushMatrix();//門扉
                glColor3f(0.4, 0.13, 0.0);
                glRotatef(-45.0, 0.0, 1.0, 0.0);
                glScaled(4.0, 9.0, 1.0);
                draw_rectangle(0);
            glPopMatrix();
            glPushMatrix();//門扉
                glColor3f(0.4, 0.13, 0.0);
                glTranslatef(8.0, 0.0, 0.0);
                glRotatef(-135.0, 0.0, 1.0, 0.0);
                glScaled(4.0, 9.0, 1.0);
                draw_rectangle(1);//反的當正
            glPopMatrix();
        glPopMatrix();

        glPushMatrix();//城牆
            glColor3f(0.92, 0.956, 1.0);
            glTranslatef(0.0, 10, 9.0);
            glScaled(21.0, 4.0, 3.0);
            draw_cube();
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.92, 0.956, 1.0);
            glTranslatef(0.0, 10, -9.0);
            glScaled(21.0, 4.0, 3.0);
            draw_cube();
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.92, 0.956, 1.0);
            glRotatef(-90.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 10, 9.0);
            glScaled(21.0, 4.0, 3.0);
            draw_cube();
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.92, 0.956, 1.0);
            glRotatef(-90.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 10, -9.0);
            glScaled(21.0, 4.0, 3.0);
            draw_cube();
        glPopMatrix();

        glPushMatrix();
            for (int i = 0; i < 4; i++) {
                glPushMatrix();
                    glColor3f(0.92, 0.956, 1.0);
                    glTranslatef(-9.0 + i * 6, 13.5, 9.0);
                    glScaled(3.0, 3.0, 3.0);
                    draw_cube();
                glPopMatrix();
            }
            for (int i = 0; i < 4; i++) {
                glPushMatrix();
                    glColor3f(0.92, 0.956, 1.0);
                    glTranslatef(-9.0 + i * 6, 13.5, -9.0);
                    glScaled(3.0, 3.0, 3.0);
                    draw_cube();
                glPopMatrix();
            }
            for (int i = 0; i < 2; i++) {
                glPushMatrix();
                    glColor3f(0.92, 0.956, 1.0);
                    glRotatef(-90.0, 0.0, 1.0, 0.0);
                    glTranslatef(-3.0 + i * 6, 13.5, 9.0);
                    glScaled(3.0, 3.0, 3.0);
                    draw_cube();
                glPopMatrix();
            }
            for (int i = 0; i < 2; i++) {
                glPushMatrix();
                    glColor3f(0.92, 0.956, 1.0);
                    glRotatef(-90.0, 0.0, 1.0, 0.0);
                    glTranslatef(-3.0 + i * 6, 13.5, -9.0);
                    glScaled(3.0, 3.0, 3.0);
                    draw_cube();
                glPopMatrix();
            }
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0, 15.5, 0.0);

            glPushMatrix();//高塔
                glEnable(GL_TEXTURE_2D);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glBindTexture(GL_TEXTURE_2D, textName[0]);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_REPLACE
                //glColor3f(0.82, 0.91, 1.0);
                glScaled(7.5, 15.0, 7.5);
                draw_cube();
                glDisable(GL_TEXTURE_2D);
            glPopMatrix();
            glPushMatrix();//窗
                glColor3f(0.988, 0.988, 0.0);
                glTranslatef(-2.0, 2.0, 3.751);
                glScaled(4.0, 5.0, 1.0);
                draw_rectangle(0);
            glPopMatrix();
                glPushMatrix();//高塔屋頂
                glEnable(GL_TEXTURE_2D);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glBindTexture(GL_TEXTURE_2D, textName[6]);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glColor3f(1.0, 1.0, 1.0);
                glTranslatef(0.0, 12, 0.0);
                glScaled(9.0, 9.0, 9.0);
                draw_triangle();
                glDisable(GL_TEXTURE_2D);
            glPopMatrix();
            glPushMatrix();//旗桿
                glColor3f(0.82, 0.82, 0.82);
                glTranslatef(0.0, 15.0, 0.0);
                glRotatef(-90.0, 1.0, 0.0, 0.0);
                glScaled(1.0, 1.0, 7.0);
                gluCylinder(cylind, 0.5, 0.5, 1.0, 12, 3);
            glPopMatrix();
            glPushMatrix();//圓球
                glColor3f(0.82, 0.82, 0.82);
                glTranslatef(0.0, 22.0, 0.0);
                glScaled(1.0, 1.0, 1.0);
                gluSphere(sphere, 1.0, 24, 12);
            glPopMatrix();
            glPushMatrix();//旗幟
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, textName[1]);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
                glColor3f(1.0, 0.18, 0.18);
                glTranslatef(0.0, 18.5, 0.0);
                glScaled(5.0, 5.0, 3.0);
                draw_banner();
                glDisable(GL_TEXTURE_2D);
            glPopMatrix();
        glPopMatrix();

        draw_tower();
        glPushMatrix();
            glRotatef(180.0, 0.0, 1.0, 0.0);
            draw_tower();
        glPopMatrix();

    glPopMatrix();
}

void fireworks() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, firework_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, firework_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, firework_mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, firework_mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, firework_mat_emission);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    if (firework_style == 1) {//直線爬升
        glPushMatrix();
            glColor3f(0.0, 1.0, 1.0);
            glTranslatef(70.0, 5.0, -20.0);
            glTranslatef(0.0, firework_heigh, 0.0);
            gluSphere(sphere, 1.0, 8, 8);
            glTranslatef(0.0, 5.0, 0.0);
            gluSphere(sphere, 1.0, 8, 8);
            glTranslatef(0.0, 5.0, 0.0);
            gluSphere(sphere, 1.0, 8, 8);
        glPopMatrix();
    }
    else if (firework_style == 2) {//一圈圈向外擴展
        glPushMatrix();
            glColor3f(0.0, 1.0, 1.0);
            glTranslatef(70.0, 80.0, -20.0);//大圓放射中心
            float angle1 = 360.0 / firework_num;
            float angle2 = 360.0 / (2 * firework_num);
            float angle3 = 360.0 / (3 * firework_num);
            for (int i = 0; i < firework_num; i++) {
                glTranslatef(firework_basis, firework_basis, 0.0);
                gluSphere(sphere, 1.0, 8, 8);
                glTranslatef(-firework_basis, -firework_basis, 0.0);
                glRotatef(angle1, 0.0, 0.0, 1.0);
            }
            for (int i = 0; i < 2 * firework_num; i++) {
                glTranslatef(2 * firework_basis, 2 * firework_basis, 0.0);
                gluSphere(sphere, 1.0, 8, 8);
                glTranslatef(-2 * firework_basis, -2 * firework_basis, 0.0);
                glRotatef(angle2, 0.0, 0.0, 1.0);
            }
            for (int i = 0; i < 3 * firework_num; i++) {
                glTranslatef(3 * firework_basis, 3 * firework_basis, 0.0);
                gluSphere(sphere, 1.0, 8, 8);
                glTranslatef(-3 * firework_basis, -3 * firework_basis, 0.0);
                glRotatef(angle3, 0.0, 0.0, 1.0);
            }
        glPopMatrix();
    }
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);//返回原本初值
}

/*--------------------------------------------------------
 * Procedure to draw a billboard, center=[x,z], width=w,
 * height = h;
 */
void draw_billboard(float x, float z, float w, float h)
{
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floor_mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, floor_mat_shininess);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);//在啟動光照系統之後，為圖元指定顏色變得不太方便

    float  v0[3], v1[3], v2[3], v3[3];

    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    /*----Compute the 4 vertices of the billboard ----*/
    v0[0] = x - (w / 2) * a[0]; v0[1] = 0.0; v0[2] = z - (w / 2) * a[2];
    v1[0] = x + (w / 2) * a[0]; v1[1] = 0.0; v1[2] = z + (w / 2) * a[2];
    v2[0] = x + (w / 2) * a[0]; v2[1] = h; v2[2] = z + (w / 2) * a[2];
    v3[0] = x - (w / 2) * a[0]; v3[1] = h; v3[2] = z - (w / 2) * a[2];

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glNormal3f(mtx[2], mtx[6], mtx[10]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex3fv(v0);
    glTexCoord2f(1.0, 1.0); glVertex3fv(v1);
    glTexCoord2f(1.0, 0.0); glVertex3fv(v2);
    glTexCoord2f(0.0, 0.0); glVertex3fv(v3);
    glEnd();

}

void motorcycle_bill() {

    /*-------Draw the billboard ----*/
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_REPLACE
    glBindTexture(GL_TEXTURE_2D, textName[4]);
    draw_billboard(30.0, -10.0, 8.0, 8.0);
    draw_billboard(5.0, -15.0, 8.0, 8.0);
    draw_billboard(-7.0, 4.0, 8.0, 8.0);
    glBindTexture(GL_TEXTURE_2D, textName[5]);
    draw_billboard(-10.0, -10.0, 8.0, 8.0);
    draw_billboard(15.0, -5.0, 8.0, 8.0);
    draw_billboard(4.0, 8.0, 8.0, 8.0);

    glDisable(GL_TEXTURE_2D);
}

void padoru_bill() {

    /*-------Draw the billboard ----*/
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_REPLACE
    glBindTexture(GL_TEXTURE_2D, textName[padoru_num]);
    glPushMatrix();
    glTranslatef(padoru_angle, 0.0, 0.0);
    //glRotatef(padoru_angle, 0.0, 1.0, 0.0);
    draw_billboard(10.0, 20.0, 8.0, 8.0);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void draw_sky() {
    glPushMatrix();
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_REPLACE
        glBindTexture(GL_TEXTURE_2D, textName[11]);
        glPushMatrix();
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glTranslatef(30.0, 20.0, 0.0);
            glColor3f(1, 1, 1);
            gluSphere(sphere, 150.0, 50, 50);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    glPopMatrix();
}

void draw_scene(int sstyle) {
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    draw_axes();

    /*glPushMatrix();
    glTranslatef(stage1_lit_position[0], stage1_lit_position[1], stage1_lit_position[2]);
    glColor3f(1.0, 1.0, 0.4392);
    gluSphere(sphere, 2.0, 8, 8);
    glPopMatrix();*/

    /*-------Rotate light position----*/
    glDisable(GL_LIGHTING);//不想要受到光照影響
    glPushMatrix();
        /*-------Draw light source as a 圓球 在世界座標移動----*/
        glPushMatrix();
            glRotatef(sun_lit_angle, 0.0, 0.0, 1.0);
            glPushMatrix();
                glTranslatef(sun_lit_position[0], sun_lit_position[1], sun_lit_position[2]);
                glColor3f(1.0, 1.0, 0.4392);
                gluSphere(sphere, 0.5, 8, 8);
            glPopMatrix();
            glEnable(GL_LIGHTING);      /* Turn on lighting */
            /*----Redefine position and direction of light0-----*/
            glLightfv(GL_LIGHT0, GL_POSITION, sun_lit_position);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_lit_diffuse);
            glLightfv(GL_LIGHT0, GL_SPECULAR, sun_lit_specular);
        glPopMatrix();

        glPushMatrix();//探照燈
            glMaterialfv(GL_FRONT, GL_AMBIENT, search_mat_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, search_mat_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, search_mat_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, search_mat_shininess);
            glMaterialfv(GL_FRONT, GL_EMISSION, search_mat_emission);
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            glColor3fv(search_color);
            glTranslatef(5.0, 4.8, 4.0);
            glTranslatef(position[0], position[1], position[2]);
            glRotatef(self_ang, 0.0, 1.0, 0.0);
            glRotatef(self_ang_ver, 1.0, 0.0, 0.0);
            glRotatef(self_ang_z, 0.0, 0.0, 1.0);//前面與直升機機身一樣
            glTranslatef(-4.0, 0.0, 0.0);//跑到機身前頭
            gluSphere(sphere, 0.5, 8, 8);//燈本人不受轉燈影響
            glRotatef(search_lit_angle, 1.0, 0.0, 0.0);//因為dir本身斜斜，所以可以看到在轉
            glLightfv(GL_LIGHT2, GL_DIFFUSE, search_lit_diffuse);
            glLightfv(GL_LIGHT2, GL_SPECULAR, search_lit_specular);
            glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, search_lit_cutoff);
            glLightfv(GL_LIGHT2, GL_POSITION, search_lit_position);
            glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, search_lit_direction);
            glMaterialfv(GL_FRONT, GL_EMISSION, emission);//返回原本初值
        glPopMatrix();

        glPushMatrix();//煙火光
            //glTranslatef(firework_lit_position[0], firework_lit_position[1], firework_lit_position[2]);
            //glTranslatef會影響GL_POSITION!!!!!
            glLightfv(GL_LIGHT3, GL_POSITION, firework_lit_position);
            glLightfv(GL_LIGHT3, GL_DIFFUSE, firework_lit_diffuse);
            glLightfv(GL_LIGHT3, GL_SPECULAR, firework_lit_specular);
        glPopMatrix();

        glPushMatrix();
            //舞台燈(紅)
            glTranslatef(40.0, 2.0, 50.0);//到圓中心
            glTranslatef(0.0, 0.0, -20.0);
            glLightfv(GL_LIGHT4, GL_POSITION, stage1_lit_position);//想要改direction，這行就要，因為是以pos為基準
            glRotatef(-30, 1.0, 0.0, 0.0);
            glRotatef(stage_lit_angle, 0.0, 0.0, 1.0);
            glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, stage1_lit_direction);
            
            //舞台燈(綠)
            glRotatef(-stage_lit_angle, 0.0, 0.0, 1.0);
            glRotatef(30, 1.0, 0.0, 0.0);
            glTranslatef(0.0, 0.0, 20.0);//到圓中心
            glRotatef(120, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 0.0, -20.0);
            glLightfv(GL_LIGHT5, GL_POSITION, stage2_lit_position);
            glRotatef(-30, 1.0, 0.0, 0.0);
            glRotatef(stage_lit_angle, 0.0, 0.0, 1.0);
            glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, stage2_lit_direction);

            //舞台燈(藍)
            glRotatef(-stage_lit_angle, 0.0, 0.0, 1.0);
            glRotatef(30, 1.0, 0.0, 0.0);
            glTranslatef(0.0, 0.0, 20.0);//到圓中心
            glRotatef(120, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 0.0, -20.0);
            glLightfv(GL_LIGHT6, GL_POSITION, stage3_lit_position);
            glRotatef(-30, 1.0, 0.0, 0.0);
            glRotatef(stage_lit_angle, 0.0, 0.0, 1.0);
            glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, stage3_lit_direction);
        glPopMatrix();
        
        glLightfv(GL_LIGHT1, GL_DIFFUSE, lamp_lit_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, lamp_lit_specular);
        
    glPopMatrix();

    draw_sky();
    fireworks();
    draw_floor();
    if (sstyle == 1 || sstyle == 2 || sstyle == 3) draw_viewvolume();
    heigh_build();
    draw_park();
    draw_bear();
    draw_castle();
    motorcycle_bill();
    padoru_bill();

    //機頭(艙)
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, helicopter_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, helicopter_mat_diffuse);
        //除了反光，顏色都用glcolor定義了
        glMaterialfv(GL_FRONT, GL_SPECULAR, helicopter_mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, helicopter_mat_shininess);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

        glTranslatef(5.0, 4.8, 4.0);
        glTranslatef(position[0], position[1], position[2]);
    
        glRotatef(self_ang, 0.0, 1.0, 0.0);
        glRotatef(self_ang_ver, 1.0, 0.0, 0.0);
        glRotatef(self_ang_z, 0.0, 0.0, 1.0);
    
        glScaled(3.0, 3.0, 3.0);
        glPushMatrix();
            glScaled(1.3, 1.0, 1.3);
            glColor3f(0.95, 0.2, 0.2);
            gluSphere(sphere, 1.0,   /* radius=5.0 */
                12,            /* composing of 12 slices 經度數*/
                12);           /* composing of 12 stacks 緯度數*/
        glPopMatrix();

        glPushMatrix();
        //機身
            glColor3f(0.68, 0.68, 0.68);
            glTranslatef(1.0, 0.0, 0.0);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            gluCylinder(cylind, 0.4, 0.1, /* radius of bottom and top circle */
                3.0,              /* height of the cylinder */
                12,               /* use 12-side polygon approximating circle*/
                3);               /* Divide it into 3 sections */

            glTranslatef(0.0, 0.3, 2.6);
            //機尾
            glPushMatrix();
                glScaled(0.1, 0.4, 0.5);
                draw_trapezoid();
            glPopMatrix();
            glPushMatrix();
                glTranslatef(-0.3, -0.3, 0.0);
                glRotatef(90.0, 0.0, 0.0, 1.0);
                glScaled(0.1, 0.4, 0.5);
                draw_trapezoid();
            glPopMatrix();
                glPushMatrix();
                glTranslatef(0.3, -0.3, 0.0);
                glRotatef(-90.0, 0.0, 0.0, 1.0);
                glScaled(0.1, 0.4, 0.5);
                draw_trapezoid();
            glPopMatrix();

        glPopMatrix();

        glPushMatrix();
        //轉軸
            glTranslatef(0.0, 0.8, 0.0);
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glColor3f(0.0, 0.0, 1.0);
            gluCylinder(cylind, 0.15, 0.15, 0.7, 12, 3);
            glTranslatef(0.0, 0.0, 0.8);
            gluSphere(sphere, 0.25, 12, 12);

            //葉片
            glRotatef(blade_ang, 0.0, 0.0, 1.0);
            draw_blade();/* draw the first blade */
            glColor3f(1.0, 1.0, 0.0);
            glRotatef(120.0, 0.0, 0.0, 1.0);    /* rotate by 120 degree, M6 coord. sys */
            draw_blade();/* draw 2nd blade */
            glColor3f(0.0, 1.0, 1.0);
            glRotatef(120.0, 0.0, 0.0, 1.0);    /* rotate by 120 degree,M7 coord. sys */
            draw_blade();/* draw 3rd blade */
        glPopMatrix();

        //腳輪子
        glPushMatrix();
            glColor3f(1.0, 1.0, 0.0);
            glPushMatrix();
                glTranslatef(0.45, -0.55, 0.7);
                glRotatef(90.0, 1.0, 0.0, 0.0);
                gluCylinder(cylind, 0.15, 0.05, 0.7, 12, 3);
                glRotatef(-90.0, 1.0, 0.0, 0.0);
                glTranslatef(0.0, -0.8, 0.0);
                glutSolidTorus(0.1, 0.15, 12, 24);
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0.45, -0.55, -0.7);
                glRotatef(90.0, 1.0, 0.0, 0.0);
                gluCylinder(cylind, 0.15, 0.05, 0.7, 12, 3);
                glRotatef(-90.0, 1.0, 0.0, 0.0);
                glTranslatef(0.0, -0.8, 0.0);
                glutSolidTorus(0.1, 0.15, 12, 24);
            glPopMatrix();
            glTranslatef(-0.45, -0.55, 0.0);
            glRotatef(90.0, 1.0, 0.0, 0.0);
            gluCylinder(cylind, 0.15, 0.05, 0.7, 12, 3);
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glTranslatef(0.0, -0.8, 0.0);
            glutSolidTorus(0.1, 0.15, 12, 24);
        glPopMatrix();

    glPopMatrix();
}

void display() {
    /*Clear previous frame and the depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    switch (style) {
    case 1:
        //左側只有顯示x軸正交平行的
        make_view(1);
        make_projection(1);
        glViewport(0, 0, width / 2, height);
        draw_scene(1);
        make_view(1);
        draw_view();
        draw_view_hel();

        //直升機視角窗口
        glViewport(width / 2, 0, width / 2, height);
        make_view(5);
        make_projection(5);
        draw_scene(5);
        break;
    case 2:
        //左側只有顯示y軸正交平行的
        make_view(2);
        make_projection(1);
        glViewport(0, 0, width / 2, height);
        draw_scene(2);
        make_view(2);
        draw_view();
        draw_view_hel();

        //直升機視角窗口
        glViewport(width / 2, 0, width / 2, height);
        make_view(5);
        make_projection(5);
        draw_scene(5);
        break;
    case 3:
        //左側只有顯示z軸正交平行的
        make_view(3);
        make_projection(1);
        glViewport(0, 0, width / 2, height);
        draw_scene(3);
        make_view(3);
        draw_view();
        draw_view_hel();

        //直升機視角窗口
        glViewport(width / 2, 0, width / 2, height);
        make_view(5);
        make_projection(5);
        draw_scene(5);
        break;
    case 4:
        //左側只有顯示眼睛看到的
        glViewport(0, 0, width / 2, height);
        make_view(4);
        make_projection(4);
        draw_scene(4);

        //直升機視角窗口
        glViewport(width / 2, 0, width / 2, height);
        make_view(5);
        make_projection(5);
        draw_scene(5);
        break;
    case 5:
        //右下窗口
        make_view(4);
        make_projection(4);
        glViewport(width / 4, 0, width / 4, height / 2);//左下位置, 寬(向左延伸)高(向上延伸)長度
        draw_scene(4);
        draw_view_hel();

        //左上窗口
        make_view(1);
        make_projection(1);
        glViewport(0, height / 2, width / 4, height / 2);
        draw_scene(1);
        make_view(1);
        draw_view();
        draw_view_hel();

        //右上窗口
        make_view(2);
        glViewport(width / 4, height / 2, width / 4, height / 2);
        draw_scene(2);
        make_view(2);
        draw_view();
        draw_view_hel();

        //左下窗口
        make_view(3);
        glViewport(0, 0, width / 4, height / 2);
        draw_scene(3);
        make_view(3);
        draw_view();
        draw_view_hel();

        //直升機視角窗口
        glViewport(width / 2, 0, width / 2, height);
        make_view(5);
        make_projection(5);
        draw_scene(4);
        break;
    }
    /*-------Swap the back buffer to the front --------*/
    glutSwapBuffers();
    return;
}

void my_reshape(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (double)w / (double)h, 1.5, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void swing_check() {
    if (det == 1) {
        swing_ang -= 2.5;
        if (swing_ang == -50.0) det = 0;
    }
    else {
        swing_ang += 2.5;
        if (swing_ang == 50.0) det = 1;
    }
}

void blade_check(int idx) {
    if (idx == 0) {
        blade_ang += 15;
        if (blade_ang > 360.0) blade_ang -= 360.0;
    }
    else {
        if (begin == 1) blade_ang += 15.0;
        if (blade_ang > 360.0) blade_ang -= 360.0;
    }
}

void bear_check() {
    if (bear_det == 1) {
        bear_ang -= 2.5;
        if (bear_ang == -40.0) bear_det = 0;
    }
    else {
        bear_ang += 2.5;
        if (bear_ang == 40.0) bear_det = 1;
    }
}

void bear_feet_check() {
    if (bear_feet_det == 1) {
        bear_feet_ang -= 2.0;
        if (bear_feet_ang == -30.0) bear_feet_det = 0;
    }
    else {
        bear_feet_ang += 2.0;
        if (bear_feet_ang == 0.0) bear_feet_det = 1;
    }
}

void light_check() {
    if (lamp_style == 1 && lamp_mod == 1) {//路燈做呼吸
        if (lamp_mod1_act == 0 && lamp_hold < 90) {//變弱
            lamp_hold++;
            lamp_lit_diffuse[0] -= 0.01;
            lamp_lit_diffuse[1] -= 0.01;
            lamp_lit_diffuse[2] -= 0.01;
            lamp_lit_specular[0] -= 0.01;
            lamp_lit_specular[1] -= 0.01;
            lamp_lit_specular[2] -= 0.01;
            if (lamp_hold == 90) {
                lamp_mod1_act = 1;
            }
        }
        else if (lamp_mod1_act == 1 && lamp_hold > 0) {
            lamp_hold--;
            lamp_lit_diffuse[0] += 0.01;
            lamp_lit_diffuse[1] += 0.01;
            lamp_lit_diffuse[2] += 0.01;
            lamp_lit_specular[0] += 0.01;
            lamp_lit_specular[1] += 0.01;
            lamp_lit_specular[2] += 0.01;
            if (lamp_hold == 0) lamp_mod1_act = 0;
        }
    }
    stage_lit_angle += 2.0;
}

void texture_ani() {
    dt += 0.03;
    if (dt > 1.0) dt = dt - 1.0;

    padoru_cnt++;
    if (padoru_cnt == 10) {
        if (padoru_mode == 0) {
            padoru_angle += 1.0;
            if (padoru_angle == 10.0) padoru_mode = 1;
        }
        else {
            padoru_angle -= 1.0;
            if (padoru_angle == -10.0) padoru_mode = 0;
        }
        padoru_num += 1;
        if (padoru_num == 11) padoru_num = 7;
        padoru_cnt = 0;
    }
}

/*--------------------------------------------------
 * Keyboard callback func. When a 'q' key is pressed,
 * the program is aborted.
 */
void my_keyboard(unsigned char key, int x, int y) {
    float  ex[3], ey[3], ez[3];
    float  helicopter_ex[3], helicopter_ey[3], helicopter_ez[3];

    if (key == 27) exit(0); //esc
    else if (key == 'E' || key == 'e') {//上升
        if (begin == 1) {//要先起飛
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            blade_check(0);
            light_check();
            texture_ani();
            if (position[1] <= 1.0) return;
            if (position[1] <= Step) {
                helicopter_eyeDy -= (position[1] - 0.0);
                for (int i = 0; i < 3; i++) helicopter_eye[i] += (position[1] - 0.0) * helicopter_u[1][i];
                position[1] += (position[1] - 0.0) * cos(-self_ang_ver * PI / 180.0);
                position[2] -= (position[1] - 0.0) * sin(-self_ang_ver * PI / 180.0);
            }
            else {
                position[1] += Step * cos(-self_ang_ver * PI / 180.0);
                position[2] -= Step * sin(-self_ang_ver * PI / 180.0);
                helicopter_eyeDy += -Step;//處理直升機視角
                for (int i = 0; i < 3; i++) helicopter_eye[i] += Step * helicopter_u[1][i];
            }
            display();
        }
    }
    else if (key == 'Q' || key == 'q') {//下升
        if (begin == 1) {//要先起飛
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            blade_check(0);
            light_check();
            texture_ani();
            if (position[1] <= 0.0) return;
            if (position[1] < Step) {
                helicopter_eyeDy += (position[1] - 0.0);
                for (int i = 0; i < 3; i++) helicopter_eye[i] -= (position[1] - 0.0) * helicopter_u[1][i];
                position[1] = 0.0;
            }
            else {
                position[1] -= Step * cos(-self_ang_ver * PI / 180.0);
                position[2] += Step * sin(-self_ang_ver * PI / 180.0);
                helicopter_eyeDy += Step;
                for (int i = 0; i < 3; i++) helicopter_eye[i] -= Step * helicopter_u[1][i];
            }
            display();
        }
    }
    else if (key == 'W' || key == 'w') {//前進
        helicopter_eyeDz += Step;
        for (int i = 0; i < 3; i++) helicopter_eye[i] -= Step * helicopter_u[2][i];
        swing_check();
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        blade_check(1);
        light_check();
        texture_ani();
        position[0] -= Step * cos(self_ang * PI / 180.0);
        position[2] += Step * sin(self_ang * PI / 180.0);
        display();
    }
    else if (key == 'S' || key == 's') {//後退
        helicopter_eyeDz += -Step;
        for (int i = 0; i < 3; i++) helicopter_eye[i] += Step * helicopter_u[2][i];
        swing_check();
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        blade_check(1);
        light_check();
        texture_ani();
        position[0] += Step * cos(self_ang * PI / 180.0);
        position[2] -= Step * sin(self_ang * PI / 180.0);
        display();
    }
    /*else if (key == 'C' || key == 'c') {//自身垂直右轉
        if (position[1] < 1.0) return;//避免扇葉穿地板
        if (begin == 1) {//要先起飛
            helicopter_eyeAngz += 10.0;
            if (helicopter_eyeAngz > 360.0) helicopter_eyeAngz -= 360.0;
            for (int i = 0; i < 3; i++) {
                helicopter_ex[i] = cos(10.0 * PI / 180.0) * helicopter_u[0][i] - sin(10.0 * PI / 180.0) * helicopter_u[1][i];
                helicopter_ey[i] = sin(10.0 * PI / 180.0) * helicopter_u[0][i] + cos(10.0 * PI / 180.0) * helicopter_u[1][i];
            }
            for (int i = 0; i < 3; i++) {
                helicopter_u[0][i] = helicopter_ex[i];
                helicopter_u[1][i] = helicopter_ey[i];
            }
            printf("%f, %f, %f\n", helicopter_u[0][0], helicopter_u[0][1], helicopter_u[0][2]);
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            blade_check(0);
            self_ang_ver -= 10.0;
            display();
        }
    }
    else if (key == 'Z' || key == 'z') {//自身垂直左轉
        if (position[1] < 1.0) return;//避免扇葉穿地板
        if (begin == 1) {//要先起飛
            helicopter_eyeAngz += -10.0;
            if (helicopter_eyeAngz < 0.0) helicopter_eyeAngz += 360.0;
            for (int i = 0; i < 3; i++) {
                helicopter_ex[i] = cos(10.0 * PI / 180.0) * helicopter_u[0][i] + sin(10.0 * PI / 180.0) * helicopter_u[1][i];
                helicopter_ey[i] = -sin(10.0 * PI / 180.0) * helicopter_u[0][i] + cos(10.0 * PI / 180.0) * helicopter_u[1][i];
            }
            for (int i = 0; i < 3; i++) {
                helicopter_u[0][i] = helicopter_ex[i];
                helicopter_u[1][i] = helicopter_ey[i];
            }
            printf("%f, %f, %f\n", helicopter_u[0][0], helicopter_u[0][1], helicopter_u[0][2]);
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            blade_check(0);
            self_ang_ver += 10.0;
            display();
        }
    }*/
    else if (key == 'D' || key == 'd') {//自身右轉
        helicopter_eyeAngy += -10.0;
        if (helicopter_eyeAngy < 0.0) helicopter_eyeAngy += 360.0;
        for (int i = 0; i < 3; i++) {
            helicopter_ex[i] = cos(10.0 * PI / 180.0) * helicopter_u[0][i] + sin(10.0 * PI / 180.0) * helicopter_u[2][i];
            helicopter_ez[i] = -sin(10.0 * PI / 180.0) * helicopter_u[0][i] + cos(10.0 * PI / 180.0) * helicopter_u[2][i];
        }
        for (int i = 0; i < 3; i++) {
            helicopter_u[0][i] = helicopter_ex[i];
            helicopter_u[2][i] = helicopter_ez[i];
        }
        swing_check();
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        if (begin == 1) blade_ang += 25.0;//抵銷右轉的10
        if (blade_ang > 360.0) blade_ang -= 360.0;
        self_ang -= 10.0;
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'A' || key == 'a') {//自身左轉
        helicopter_eyeAngy += 10.0;
        if (helicopter_eyeAngy > 360.0) helicopter_eyeAngy -= 360.0;
        for (int i = 0; i < 3; i++) {
            helicopter_ex[i] = cos(10.0 * PI / 180.0) * helicopter_u[0][i] - sin(10.0 * PI / 180.0) * helicopter_u[2][i];
            helicopter_ez[i] = sin(10.0 * PI / 180.0) * helicopter_u[0][i] + cos(10.0 * PI / 180.0) * helicopter_u[2][i];
        }
        for (int i = 0; i < 3; i++) {
            helicopter_u[0][i] = helicopter_ex[i];
            helicopter_u[2][i] = helicopter_ez[i];
        }
        swing_check();
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        if (begin == 1) blade_ang += 5.0;//抵銷左轉的10
        if (blade_ang > 360.0) blade_ang -= 360.0;
        self_ang += 10.0;
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'O' || key == 'o') {//繞某中心的左轉(公轉)
        for (int i = 0; i < 18; i++) {
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            position[0] -= Step * cos(self_ang * PI / 180.0);
            position[2] += Step * sin(self_ang * PI / 180.0);
            self_ang += 5.0;
            light_check();
            texture_ani();
            
            if (begin == 1) blade_ang += 10.0;//抵銷左轉的5
            if (blade_ang > 360.0) blade_ang -= 360.0;

            helicopter_eyeAngy += -90.0;
            if (helicopter_eyeAngy < 0.0) helicopter_eyeAngy += 360.0;
            for (int j = 0; j < 3; j++) {
                helicopter_ex[j] = cos(90.0 * PI / 180.0) * helicopter_u[0][j] + sin(90.0 * PI / 180.0) * helicopter_u[2][j];
                helicopter_ez[j] = -sin(90.0 * PI / 180.0) * helicopter_u[0][j] + cos(90.0 * PI / 180.0) * helicopter_u[2][j];
            }
            for (int j = 0; j < 3; j++) {
                helicopter_u[0][j] = helicopter_ex[j];
                helicopter_u[2][j] = helicopter_ez[j];
            }
            //因為飛一小段距
            helicopter_eyeDx += Step * cos(5.0 * PI / 180.0);
            for (int j = 0; j < 3; j++) helicopter_eye[j] -= Step * cos(5.0 * PI / 180.0) * helicopter_u[0][j];

            helicopter_eyeAngy += 5.0;
            if (helicopter_eyeAngy > 360.0) helicopter_eyeAngy -= 360.0;
            for (int j = 0; j < 3; j++) {
                helicopter_ex[j] = cos(5.0 * PI / 180.0) * helicopter_u[0][j] - sin(5.0 * PI / 180.0) * helicopter_u[2][j];
                helicopter_ez[j] = sin(5.0 * PI / 180.0) * helicopter_u[0][j] + cos(5.0 * PI / 180.0) * helicopter_u[2][j];
            }
            for (int j = 0; j < 3; j++) {
                helicopter_u[0][j] = helicopter_ex[j];
                helicopter_u[2][j] = helicopter_ez[j];
            }

            helicopter_eyeAngy += 90.0;
            if (helicopter_eyeAngy > 360.0) helicopter_eyeAngy -= 360.0;
            for (int j = 0; j < 3; j++) {
                helicopter_ex[j] = cos(90.0 * PI / 180.0) * helicopter_u[0][j] - sin(90.0 * PI / 180.0) * helicopter_u[2][j];
                helicopter_ez[j] = sin(90.0 * PI / 180.0) * helicopter_u[0][j] + cos(90.0 * PI / 180.0) * helicopter_u[2][j];
            }
            for (int j = 0; j < 3; j++) {
                helicopter_u[0][j] = helicopter_ex[j];
                helicopter_u[2][j] = helicopter_ez[j];
            }

            display();
        }
    }
    else if (key == 'P' || key == 'p') {//繞某中心的右轉(公轉)
        for (int i = 0; i < 18; i++) {

            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            position[0] -= Step * cos(self_ang * PI / 180.0);
            position[2] += Step * sin(self_ang * PI / 180.0);
            self_ang -= 5.0;
            light_check();
            texture_ani();
            
            if (begin == 1) blade_ang += 20.0;//抵銷右轉的5
            if (blade_ang > 360.0) blade_ang -= 360.0;

            helicopter_eyeAngy += -90.0;
            if (helicopter_eyeAngy < 0.0) helicopter_eyeAngy += 360.0;
            for (int j = 0; j < 3; j++) {
                helicopter_ex[j] = cos(90.0 * PI / 180.0) * helicopter_u[0][j] - sin(90.0 * PI / 180.0) * helicopter_u[2][j];
                helicopter_ez[j] = sin(90.0 * PI / 180.0) * helicopter_u[0][j] + cos(90.0 * PI / 180.0) * helicopter_u[2][j];
            }
            for (int j = 0; j < 3; j++) {
                helicopter_u[0][j] = helicopter_ex[j];
                helicopter_u[2][j] = helicopter_ez[j];
            }
            //因為飛一小段距
            helicopter_eyeDx += -Step * cos(5.0 * PI / 180.0);
            for (int j = 0; j < 3; j++) helicopter_eye[j] += Step * cos(5.0 * PI / 180.0) * helicopter_u[0][j];

            helicopter_eyeAngy -= 5.0;
            if (helicopter_eyeAngy > 360.0) helicopter_eyeAngy += 360.0;
            for (int j = 0; j < 3; j++) {
                helicopter_ex[j] = cos(5.0 * PI / 180.0) * helicopter_u[0][j] + sin(5.0 * PI / 180.0) * helicopter_u[2][j];
                helicopter_ez[j] = -sin(5.0 * PI / 180.0) * helicopter_u[0][j] + cos(5.0 * PI / 180.0) * helicopter_u[2][j];
            }
            for (int j = 0; j < 3; j++) {
                helicopter_u[0][j] = helicopter_ex[j];
                helicopter_u[2][j] = helicopter_ez[j];
            }

            helicopter_eyeAngy += 90.0;
            if (helicopter_eyeAngy > 360.0) helicopter_eyeAngy -= 360.0;
            for (int j = 0; j < 3; j++) {
                helicopter_ex[j] = cos(90.0 * PI / 180.0) * helicopter_u[0][j] + sin(90.0 * PI / 180.0) * helicopter_u[2][j];
                helicopter_ez[j] = -sin(90.0 * PI / 180.0) * helicopter_u[0][j] + cos(90.0 * PI / 180.0) * helicopter_u[2][j];
            }
            for (int j = 0; j < 3; j++) {
                helicopter_u[0][j] = helicopter_ex[j];
                helicopter_u[2][j] = helicopter_ez[j];
            }

            display();
        }
    }
    else if (key == 'l') {//降落(高度夠高時才會頭先下)
        double len_heigh = position[1];//輪子底到地面距離
        int head_down = len_heigh / 2;//頭先下去走的距離
        head_down = head_down / 0.1;//次數
        if (self_ang_ver != 0.0) {//要先導正->不會用到了= =
            self_ang_ver = (int)self_ang_ver % 360;
            if (self_ang_ver > 180) self_ang_ver -= 360;
            if (self_ang_ver < -180) self_ang_ver += 360;
            for (int i = 0; ; i++) {
                if (self_ang_ver == 0.0) break;
                swing_check();
                if (bear_act == 0) bear_check();
                else bear_feet_check();
                blade_check(0);
                if (self_ang_ver < 0) self_ang_ver += 5.0;
                else self_ang_ver -= 5.0;
                light_check();
                texture_ani();
                display();
            }
        }
        if (head_down != 0) {
            for (int i = 0; i < 30; i++) {//頭轉下去
                helicopter_eyeDz += 0.005 * cos(1.0 * PI / 180.0);
                for (int i = 0; i < 3; i++) helicopter_eye[i] -= 0.005 * cos(1.0 * PI / 180.0) * helicopter_u[2][i];

                helicopter_eyeAngx += 1.0;
                if (helicopter_eyeAngx < 0.0) helicopter_eyeAngx += 360.0;
                for (int i = 0; i < 3; i++) {
                    helicopter_ey[i] = cos(1.0 * PI / 180.0) * helicopter_u[1][i] - sin(1.0 * PI / 180.0) * helicopter_u[2][i];
                    helicopter_ez[i] = sin(1.0 * PI / 180.0) * helicopter_u[1][i] + cos(1.0 * PI / 180.0) * helicopter_u[2][i];
                }
                for (int i = 0; i < 3; i++) {
                    helicopter_u[1][i] = helicopter_ey[i];
                    helicopter_u[2][i] = helicopter_ez[i];
                }

                swing_check();
                if (bear_act == 0) bear_check();
                else bear_feet_check();
                blade_check(0);
                self_ang_z += 1;
                light_check();
                texture_ani();
                display();
            }
            for (int i = 0; i < head_down; i++) {//頭先下去
                helicopter_eyeDy += (0.1 * cos(30.0 * PI / 180.0));
                for (int i = 0; i < 3; i++) helicopter_eye[i] -= (0.1 * cos(30.0 * PI / 180.0)) * helicopter_u[1][i];
                helicopter_eyeDz += (0.1 * sin(30.0 * PI / 180.0));
                for (int i = 0; i < 3; i++) helicopter_eye[i] -= (0.1 * sin(30.0 * PI / 180.0)) * helicopter_u[2][i];

                swing_check();
                if (bear_act == 0) bear_check();
                else bear_feet_check();
                blade_check(0);
                position[1] -= 0.1;
                light_check();
                texture_ani();
                display();
            }
            for (int i = 0; i < 30; i++) {//尾再下去到頭位置
                //helicopter_eyeDz += -0.005 * cos(1.0 * PI / 180.0);
                //for (int i = 0; i < 3; i++) helicopter_eye[i] = 0.005 * cos(1.0 * PI / 180.0) * helicopter_u[2][i];

                helicopter_eyeAngx += -1.0;//逆時針
                if (helicopter_eyeAngx > 360.0) helicopter_eyeAngx -= 360.0;
                for (int i = 0; i < 3; i++) {
                    helicopter_ey[i] = cos(1.0 * PI / 180.0) * helicopter_u[1][i] + sin(1.0 * PI / 180.0) * helicopter_u[2][i];
                    helicopter_ez[i] = -sin(1.0 * PI / 180.0) * helicopter_u[1][i] + cos(1.0 * PI / 180.0) * helicopter_u[2][i];
                }
                for (int i = 0; i < 3; i++) {
                    helicopter_u[1][i] = helicopter_ey[i];
                    helicopter_u[2][i] = helicopter_ez[i];
                }

                swing_check();
                if (bear_act == 0) bear_check();
                else bear_feet_check();
                blade_check(0);
                self_ang_z -= 1;
                light_check();
                texture_ani();
                display();
            }
        }
        for (int i = 0; ; i++) {
            if (position[1] <= 0.0) break;
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            blade_check(0);
            light_check();
            texture_ani();
            if (position[1] < 0.1) {
                helicopter_eyeDy += (position[1] - 0.0);
                for (int i = 0; i < 3; i++) helicopter_eye[i] -= (position[1]-0.0) * helicopter_u[1][i];
                position[1] = 0.0;
            }
            else {
                helicopter_eyeDy += 0.1;
                for (int i = 0; i < 3; i++) helicopter_eye[i] -= 0.1 * helicopter_u[1][i];
                position[1] -= 0.1;
            }
            display();
        }
        if (begin == 1) {
            for (int i = 0; i < 150; i++) {
                swing_check();
                if (bear_act == 0) bear_check();
                else bear_feet_check();
                blade_ang += 0.1 * (150 - i);//慢慢變慢
                if (blade_ang > 360.0) blade_ang -= 360.0;
                light_check();
                texture_ani();
                display();
            }
        }
        begin = 0;
    }
    else if (key == 'L') {//起飛
        if (begin == 1) return;//已經在飛行狀態
        begin = 1;
        for (long long i = 0; i < 150; i++) {
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            blade_ang += 0.1 * i;//慢慢變快
            if (blade_ang > 360.0) blade_ang -= 360.0;
            light_check();
            texture_ani();
            display();
        }
        for (int i = 0; i < 20; i++) {//頭轉上去
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            blade_check(0);
            self_ang_z -= 1;
            //向前滑行
            position[0] -= 0.05 * cos(self_ang * PI / 180.0);
            position[1] += 0.05;
            position[2] += 0.05 * sin(self_ang * PI / 180.0);
            light_check();
            texture_ani();

            //因為滑行一小段距
            helicopter_eyeDy += -0.05;
            for (int i = 0; i < 3; i++) helicopter_eye[i] += 0.05 * helicopter_u[1][i];
            helicopter_eyeDz += 0.15;
            for (int i = 0; i < 3; i++) helicopter_eye[i] -= sqrt(0.05 * cos(self_ang * PI / 180.0) * 0.05 * cos(self_ang * PI / 180.0) + 0.05 * sin(self_ang * PI / 180.0) * 0.05 * sin(self_ang * PI / 180.0)) * helicopter_u[2][i];

            helicopter_eyeAngx += -1.0;
            if (helicopter_eyeAngx < 0.0) helicopter_eyeAngx += 360.0;
            for (int i = 0; i < 3; i++) {
                helicopter_ey[i] = cos(1.0 * PI / 180.0) * helicopter_u[1][i] + sin(1.0 * PI / 180.0) * helicopter_u[2][i];
                helicopter_ez[i] = -sin(1.0 * PI / 180.0) * helicopter_u[1][i] + cos(1.0 * PI / 180.0) * helicopter_u[2][i];
            }
            for (int i = 0; i < 3; i++) {
                helicopter_u[1][i] = helicopter_ey[i];
                helicopter_u[2][i] = helicopter_ez[i];
            }
            display();
        }
        for (int i = 0; i < 20; i++) {//頭先上去
            helicopter_eyeDy += -(0.1 * cos(20.0 * PI / 180.0));
            for (int i = 0; i < 3; i++) helicopter_eye[i] += (0.1 * cos(20.0 * PI / 180.0)) * helicopter_u[1][i];
            helicopter_eyeDz += (0.1 * sin(20.0 * PI / 180.0));
            for (int i = 0; i < 3; i++) helicopter_eye[i] -= (0.1 * sin(20.0 * PI / 180.0)) * helicopter_u[2][i];
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            blade_check(0);
            position[1] += 0.1;
            light_check();
            texture_ani();
            display();
        }
        for (int i = 0; i < 20; i++) {//尾再上去到頭位置
            //helicopter_eyeDz += 0.005 * cos(1.0 * PI / 180.0);
            //for (int i = 0; i < 3; i++) helicopter_eye[i] -= 0.005 * cos(1.0 * PI / 180.0) * helicopter_u[2][i];

            helicopter_eyeAngx += 1.0;//逆時針
            if (helicopter_eyeAngx > 360.0) helicopter_eyeAngx -= 360.0;
            for (int i = 0; i < 3; i++) {
                helicopter_ey[i] = cos(1.0 * PI / 180.0) * helicopter_u[1][i] - sin(1.0 * PI / 180.0) * helicopter_u[2][i];
                helicopter_ez[i] = sin(1.0 * PI / 180.0) * helicopter_u[1][i] + cos(1.0 * PI / 180.0) * helicopter_u[2][i];
            }
            for (int i = 0; i < 3; i++) {
                helicopter_u[1][i] = helicopter_ey[i];
                helicopter_u[2][i] = helicopter_ez[i];
            }
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            blade_check(0);
            self_ang_z += 1;
            light_check();
            texture_ani();
            display();
        }
        for (int i = 0; i < 30; i++) {
            helicopter_eyeDy += -0.1;
            for (int i = 0; i < 3; i++) helicopter_eye[i] += 0.1 * helicopter_u[1][i];
            swing_check();
            if (bear_act == 0) bear_check();
            else bear_feet_check();
            blade_check(0);
            position[1] += 0.1;
            light_check();
            texture_ani();
            display();
        }
    }
    else if (key == 'B') {//熊熊坐起/躺下
        if (bear_act == 0) {//躺下狀態
            bear_act = 1;
            if (bear_ang > 0) {
                for (int i = 0;; i++) {
                    if (bear_ang == 0.0) break;
                    swing_check();
                    blade_check(1);
                    bear_ang -= 2.5;
                    light_check();
                    texture_ani();
                    display();
                }
            }
            else {
                for (int i = 0;; i++) {
                    if (bear_ang == 0.0) break;
                    swing_check();
                    blade_check(1);
                    bear_ang += 2.5;
                    light_check();
                    texture_ani();
                    display();
                }
            }
            bear_act_move = 1.0;
            for (int i = 0; i < 30; i++) {
                swing_check();
                blade_check(1);
                bear_ang_ver -= 3;
                light_check();
                texture_ani();
                display();
            }
        }
        else {
            for (int i = 0; i < 30; i++) {
                swing_check();
                blade_check(1);
                bear_ang_ver += 3;
                light_check();
                texture_ani();
                display();
            }
            bear_act_move = 0.0;
            bear_act = 0;
        }
    }
    else if (key == 'b') {//熊熊旋轉
        bear_ang_hoz += 10.0;
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }

    else if (key == 'k') {//眼睛座標下移
        eyeDy += 0.5;
        for (int i = 0; i < 3; i++) eye[i] -= 0.5 * u[1][i];
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'K') {//眼睛座標上移
        eyeDy += -0.5;
        for (int i = 0; i < 3; i++) eye[i] += 0.5 * u[1][i];
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'j') {//眼睛座標右移
        eyeDx += -0.5;
        for (int i = 0; i < 3; i++) eye[i] += 0.5 * u[0][i];
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'J') {//眼睛座標左移
        eyeDx += 0.5;
        for (int i = 0; i < 3; i++) eye[i] -= 0.5 * u[0][i];
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'h') {//眼睛座標前進
        eyeDz += 0.5;
        for (int i = 0; i < 3; i++) eye[i] -= 0.5 * u[2][i];
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'H') {//眼睛座標後退
        eyeDz += -0.5;
        for (int i = 0; i < 3; i++) eye[i] += 0.5 * u[2][i];
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'i') {//pitching 以X方向為旋轉軸
        eyeAngx += 5.0;//順時針
        if (eyeAngx > 360.0) eyeAngx -= 360.0;
        ey[0] = u[1][0] * cv - u[2][0] * sv;
        ey[1] = u[1][1] * cv - u[2][1] * sv;
        ey[2] = u[1][2] * cv - u[2][2] * sv;

        ez[0] = u[2][0] * cv + u[1][0] * sv;
        ez[1] = u[2][1] * cv + u[1][1] * sv;
        ez[2] = u[2][2] * cv + u[1][2] * sv;

        for (int i = 0; i < 3; i++) {
            u[1][i] = ey[i];
            u[2][i] = ez[i];
        }
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'I') {
        eyeAngx += -5.0;//逆時針
        if (eyeAngx < 0.0) eyeAngx += 360.0;
        ey[0] = u[1][0] * cv + u[2][0] * sv;
        ey[1] = u[1][1] * cv + u[2][1] * sv;
        ey[2] = u[1][2] * cv + u[2][2] * sv;

        ez[0] = u[2][0] * cv - u[1][0] * sv;
        ez[1] = u[2][1] * cv - u[1][1] * sv;
        ez[2] = u[2][2] * cv - u[1][2] * sv;

        for (int i = 0; i < 3; i++) {
            u[1][i] = ey[i];
            u[2][i] = ez[i];
        }
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'u') {//heading Y軸
        eyeAngy += 5.0;//逆時針
        if (eyeAngy > 360.0) eyeAngy -= 360.0;
        for (int i = 0; i < 3; i++) {
            ex[i] = cv * u[0][i] - sv * u[2][i];
            ez[i] = sv * u[0][i] + cv * u[2][i];
        }
        for (int i = 0; i < 3; i++) {
            u[0][i] = ex[i];
            u[2][i] = ez[i];
        }
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'U') {
        eyeAngy += -5.0;//順時針
        if (eyeAngy < 0.0) eyeAngy += 360.0;
        for (int i = 0; i < 3; i++) {
            ex[i] = cv * u[0][i] + sv * u[2][i];
            ez[i] = -sv * u[0][i] + cv * u[2][i];
        }
        for (int i = 0; i < 3; i++) {
            u[0][i] = ex[i];
            u[2][i] = ez[i];
        }
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'y') {//rolling Z軸
        eyeAngz += 5.0;//順時針
        if (eyeAngz > 360.0) eyeAngz -= 360.0;
        for (int i = 0; i < 3; i++) {
            ex[i] = cv * u[0][i] - sv * u[1][i];
            ey[i] = sv * u[0][i] + cv * u[1][i];
        }
        for (int i = 0; i < 3; i++) {
            u[0][i] = ex[i];
            u[1][i] = ey[i];
        }
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'Y') {
        eyeAngz += -5.0;//逆時針
        if (eyeAngz < 0.0) eyeAngz += 360.0;
        for (int i = 0; i < 3; i++) {
            ex[i] = cv * u[0][i] + sv * u[1][i];
            ey[i] = -sv * u[0][i] + cv * u[1][i];
        }
        for (int i = 0; i < 3; i++) {
            u[0][i] = ex[i];
            u[1][i] = ey[i];
        }
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'x') {//zoom in 透視/平行投影前進
        zoom -= 1;
        windowclipping[0] = ori_windowclipping[0] * zoom / 90;
        windowclipping[1] = ori_windowclipping[1] * zoom / 90;
        windowclipping[2] = ori_windowclipping[2] * zoom / 90;
        windowclipping[3] = ori_windowclipping[3] * zoom / 90;
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == 'X') {//zoom out 透視/平行投影後退
        zoom += 1;
        windowclipping[0] = ori_windowclipping[0] * zoom / 90;
        windowclipping[1] = ori_windowclipping[1] * zoom / 90;
        windowclipping[2] = ori_windowclipping[2] * zoom / 90;
        windowclipping[3] = ori_windowclipping[3] * zoom / 90;
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == '1') {//x軸正交平行投影
        style = 1;
    }
    else if (key == '2') {//y軸正交平行投影
        style = 2;
    }
    else if (key == '3') {//z軸正交平行投影
        style = 3;
    }
    else if (key == '4') {//透視投影
        style = 4;
    }
    else if (key == '5') {//割四個畫面
        style = 5;
    }
    else if (key == ' ') {
        sun_lit_angle += 1;
        if (sun_lit_angle % 360 == 0) {//清晨
            glEnable(GL_LIGHT0);
            sun_lit_diffuse[0] = 0.8;
            sun_lit_diffuse[1] = 0.8;
            sun_lit_diffuse[2] = 0.8;
            sun_lit_specular[0] = 0.1;
            sun_lit_specular[1] = 0.1;
            sun_lit_specular[2] = 0.1;
            fog_color[0] = 0.760784;
            fog_color[1] = 0.8745098;
            fog_color[2] = 1.0;
        }
        else if (sun_lit_angle % 360 <= 80 && sun_lit_angle % 360 > 0) {
            sun_lit_diffuse[0] += 0.0025;
            sun_lit_diffuse[1] += 0.0025;
            sun_lit_diffuse[2] += 0.0025;
            sun_lit_specular[0] += 0.01125;
            sun_lit_specular[1] += 0.01125;
            sun_lit_specular[2] += 0.01125;
            fog_color[0] -= 0.0045098;
            fog_color[1] -= 0.0059313725;
            fog_color[2] -= 0.0075;
        }
        else if (sun_lit_angle % 360 == 81) {//正午
            sun_lit_diffuse[0] = 1.0;
            sun_lit_diffuse[1] = 1.0;
            sun_lit_diffuse[2] = 1.0;
            sun_lit_specular[0] = 1.0;
            sun_lit_specular[1] = 1.0;
            sun_lit_specular[2] = 1.0;
            fog_color[0] = 0.4;
            fog_color[1] = 0.4;
            fog_color[2] = 0.4;
        }
        else if (sun_lit_angle % 360 <= 180 && sun_lit_angle % 360 > 81) {
            sun_lit_diffuse[1] -= 0.00404;
            sun_lit_diffuse[2] -= 0.00808;
            sun_lit_specular[1] -= 0.00202;
            sun_lit_specular[2] -= 0.00404;
            fog_color[0] += 0.006;
            fog_color[1] += 0.004588235;
            fog_color[2] += 0.003215686;
        }
        else if (sun_lit_angle % 360 <= 180) {//午後
            sun_lit_diffuse[0] = 1.0;
            sun_lit_diffuse[1] = 0.6;
            sun_lit_diffuse[2] = 0.2;
            sun_lit_specular[0] = 1.0;
            sun_lit_specular[1] = 0.8;
            sun_lit_specular[2] = 0.6;
            fog_color[0] = 1.0;
            fog_color[1] = 0.8588235294;
            fog_color[2] = 0.7215686275;
        }
        else if(sun_lit_angle % 360 > 180 && sun_lit_angle % 360 < 270) {//夜上半
            glDisable(GL_LIGHT0);
            fog_color[0] -= 0.00766885;
            fog_color[1] -= 0.0061002181;
            fog_color[2] -= 0.00457516364;
        }
        else if (sun_lit_angle % 360 == 270) {//深夜
            glDisable(GL_LIGHT0);
            fog_color[0] = 0.3098039;
            fog_color[1] = 0.3098039;
            fog_color[2] = 0.3098039;
        }
        else if (sun_lit_angle % 360 > 270 && sun_lit_angle % 360 < 360) {//夜下半
            glDisable(GL_LIGHT0);
            fog_color[0] += 0.00501089;
            fog_color[1] += 0.00627451;
            fog_color[2] += 0.007668845;
        }
        glFogfv(GL_FOG_COLOR, fog_color);
    }
    else if (key == '.') {//開關路燈
        if (lamp_style == 1) {
            lamp_style = 0;
            glDisable(GL_LIGHT1);
        }
        else {
            lamp_style = 1;
            glEnable(GL_LIGHT1);
        }
    }
    else if (key == ',') {//切換路燈兩種模式，常亮、呼吸燈
        if (lamp_mod == 1) {//換回常亮
            lamp_mod = 0;
            lamp_lit_diffuse[0] = 0.5;
            lamp_lit_diffuse[1] = 0.5;
            lamp_lit_diffuse[2] = 0.5;
            lamp_lit_specular[0] = 1.0;
            lamp_lit_specular[1] = 1.0;
            lamp_lit_specular[2] = 0.8;
        }
        else {//換回呼吸燈
            lamp_mod = 1;
            lamp_mod1_act = 0;
            lamp_hold = 0;
            lamp_lit_diffuse[0] = 1.0;
            lamp_lit_diffuse[1] = 1.0;
            lamp_lit_diffuse[2] = 1.0;
            lamp_lit_specular[0] = 1.0;
            lamp_lit_specular[1] = 1.0;
            lamp_lit_specular[2] = 1.0;
        }
    }
    else if (key == '=') {//轉動探照燈
        search_lit_angle += 5.0;
    }
    else if (key == '-') {//換探照燈顏色
        if (search_lit_diffuse[0] == 1.0) {
            search_mat_emission[0] = 0.79607843;
            search_mat_emission[1] = 0.5607843;
            search_mat_emission[2] = 1.0;
            search_lit_diffuse[0] = 0.8;
            search_lit_diffuse[1] = 0.0;
            search_lit_diffuse[2] = 0.8;
            search_lit_specular[0] = 0.8;
            search_lit_specular[1] = 0.0;
            search_lit_specular[2] = 0.8;
            search_color[0] = 0.8;
            search_color[1] = 0.0;
            search_color[2] = 0.8;
        }
        else {
            search_mat_emission[0] = 1.0;
            search_mat_emission[1] = 1.0;
            search_mat_emission[2] = 0.6588235;
            search_lit_diffuse[0] = 1.0;
            search_lit_diffuse[1] = 1.0;
            search_lit_diffuse[2] = 0.0;
            search_lit_specular[0] = 1.0;
            search_lit_specular[1] = 1.0;
            search_lit_specular[2] = 0.0;
            search_color[0] = 1.0;
            search_color[1] = 1.0;
            search_color[2] = 0.0;
        }
    }
    else if (key == '[') {//探照燈光照廣度--
        search_lit_cutoff -= 5.0;
        if (search_lit_cutoff == -5.0) search_lit_cutoff = 0.0;
    }
    else if (key == ']') {//探照燈光照廣度++
        search_lit_cutoff += 5.0;
        if (search_lit_cutoff == 65.0) search_lit_cutoff = 60.0;
    }
    else if (key == '0') {//放煙火
        glEnable(GL_LIGHT3);
        for (int i = 0; i < 13; i++) {
            firework_lit_diffuse[0] += 0.05067873;
            firework_lit_diffuse[1] += 0.076923;
            firework_lit_specular[0] += 0.05067873;
            firework_lit_specular[1] += 0.076923;
            firework_lit_position[1] += 5.0;
            firework_style = 1;
            firework_heigh += 5.0;
            for (int j = 0; j < 10; j++) {
                swing_check();
                blade_check(1);
                if (bear_act == 0) bear_check();
                else bear_feet_check();
                display();
            }
        }
        firework_lit_diffuse[0] = 0.6588235;
        firework_lit_diffuse[1] = 1.0;
        firework_lit_diffuse[2] = 1.0;
        firework_lit_specular[0] = 0.6588235;
        firework_lit_specular[1] = 1.0;
        firework_lit_specular[2] = 1.0;
        firework_lit_position[1] += 5.0;
        for (int i = 0; i < 5; i++) {
            firework_style = 2;
            firework_num += 5;
            firework_basis += 3.0;
            for (int j = 0; j < 15; j++) {
                swing_check();
                blade_check(1);
                if (bear_act == 0) bear_check();
                else bear_feet_check();
                light_check();
                texture_ani();
                display();
            }
        }
        glDisable(GL_LIGHT3);
        firework_style = 0;
        firework_heigh = 0.0;
        firework_lit_diffuse[0] = 0.0;
        firework_lit_diffuse[1] = 0.0;
        firework_lit_diffuse[2] = 1.0;
        firework_lit_specular[0] = 0.0;
        firework_lit_specular[1] = 0.0;
        firework_lit_specular[2] = 1.0;
        firework_lit_position[1] = 10.0;
        firework_basis = 3.0;
        firework_num = 0;
        swing_check();
        blade_check(1);
        if (bear_act == 0) bear_check();
        else bear_feet_check();
        light_check();
        texture_ani();
        display();
    }
    else if (key == '/') {//開關舞台燈
        if (stage_on == 0) {//開啟
            stage_on = 1;
            glEnable(GL_LIGHT4);
            glEnable(GL_LIGHT5);
            glEnable(GL_LIGHT6);
        }
        else {
            stage_on = 0;
            glDisable(GL_LIGHT4);
            glDisable(GL_LIGHT5);
            glDisable(GL_LIGHT6);
        }
    }
    else if (key == ';') {//霧濃度減
        fog_density -= 0.05;
        if (fog_density < 0.05)  fog_density = 0.05;
        glFogf(GL_FOG_DENSITY, fog_density);
    }
    else if (key == '\'') {//霧濃度加
        fog_density += 0.05;
        if (fog_density > 0.5)  fog_density = 0.5;
        glFogf(GL_FOG_DENSITY, fog_density);
    }
    else if (key == 'm' || key == 'M') {//霧換三種模式
        if (fog_mode == 3) {
            fog_mode = 0;
            glDisable(GL_FOG);
        }
        else if (fog_mode == 0) {
            glEnable(GL_FOG);
            fog_mode = 1;
            glFogi(GL_FOG_MODE, GL_EXP);
        }
        else if (fog_mode == 1) {
            fog_mode = 2;
            glFogi(GL_FOG_MODE, GL_EXP2); 
        }
        else if (fog_mode == 2) {
            fog_mode = 3;
            glFogi(GL_FOG_MODE, GL_LINEAR);
        }
    }
}

/*---------------------------------------------
 * Idle event callback function, increase blade and arm
 * rotation angles.
 */
void idle_func()
{
    if (begin == 1) blade_ang += 15.0;//還沒起飛前不要轉
    if (blade_ang > 360.0) blade_ang -= 360.0;
    swing_check();
    if (bear_act == 0) bear_check();
    else bear_feet_check();
    if (lamp_style == 1 && lamp_mod == 1) {//路燈做呼吸
        if (lamp_mod1_act == 0 && lamp_hold < 90) {//變弱
            lamp_hold++;
            lamp_lit_diffuse[0] -= 0.01;
            lamp_lit_diffuse[1] -= 0.01;
            lamp_lit_diffuse[2] -= 0.01;
            lamp_lit_specular[0] -= 0.01;
            lamp_lit_specular[1] -= 0.01;
            lamp_lit_specular[2] -= 0.01;
            if (lamp_hold == 90) {
                lamp_mod1_act = 1;
            }
        }
        else if (lamp_mod1_act == 1 && lamp_hold > 0) {
            lamp_hold--;
            lamp_lit_diffuse[0] += 0.01;
            lamp_lit_diffuse[1] += 0.01;
            lamp_lit_diffuse[2] += 0.01;
            lamp_lit_specular[0] += 0.01;
            lamp_lit_specular[1] += 0.01;
            lamp_lit_specular[2] += 0.01;
            if (lamp_hold == 0) lamp_mod1_act = 0;
        }
    }
    stage_lit_angle += 2.0;
    dt += 0.03;
    if (dt > 1.0) dt = dt - 1.0;
    padoru_cnt++;
    if (padoru_cnt == 10) {
        if (padoru_mode == 0) {
            padoru_angle += 1.0;
            if (padoru_angle == 10.0) padoru_mode = 1;
        }
        else {
            padoru_angle -= 1.0;
            if (padoru_angle == -10.0) padoru_mode = 0;
        }
        padoru_num += 1;
        if (padoru_num == 11) padoru_num = 7;
        padoru_cnt = 0;
    }
    
    display();
}


/*---------------------------------------------------
 * Main procedure which defines the graphics environment,
 * the operation style, and the callback func's.
 */
void main(int argc, char** argv)
{
    /*-----Initialize the GLUT environment-------*/
    glutInit(&argc, argv);

    /*-----Depth buffer is used, be careful !!!----*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("helicopter_view");

    myinit();      /*---Initialize other state varibales----*/

    /*----Associate callback func's whith events------*/
    glutDisplayFunc(display);
    glutIdleFunc(idle_func);    /* Add idle event */
    glutReshapeFunc(my_reshape);
    glutKeyboardFunc(my_keyboard);

    glutMainLoop();
}
