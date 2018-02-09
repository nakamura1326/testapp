/* NUMBER PLACE (数独) */

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <limits.h>
#include "sudoku_util.h"

static char LEFT_TOP_RULEDLINE[MB_LEN_MAX];
static char LEFT_BOTTOM_RULEDLINE[MB_LEN_MAX];
static char RIGHT_TOP_RULEDLINE[MB_LEN_MAX];
static char RIGHT_BOTTOM_RULEDLINE[MB_LEN_MAX];
static char LEFT_SIDE_RULEDLINE[MB_LEN_MAX];
static char LEFT_SIDE_RULEDLINE_BOLD[MB_LEN_MAX];
static char RIGHT_SIDE_RULEDLINE[MB_LEN_MAX];
static char RIGHT_SIDE_RULEDLINE_BOLD[MB_LEN_MAX];
static char TOP_SIDE_RULEDLINE[MB_LEN_MAX];
static char TOP_SIDE_RULEDLINE_BOLD[MB_LEN_MAX];
static char BOTTOM_SIDE_RULEDLINE[MB_LEN_MAX];
static char BOTTOM_SIDE_RULEDLINE_BOLD[MB_LEN_MAX];
static char VERTICAL_RULEDLINE[MB_LEN_MAX];
static char VERTICAL_RULEDLINE_BOLD[MB_LEN_MAX];
static char HORIZONTAL_RULEDLINE[MB_LEN_MAX];
static char HORIZONTAL_RULEDLINE_BOLD[MB_LEN_MAX];
static char CROSS_POINT_RULEDLINE[MB_LEN_MAX];
static char CROSS_POINT_RULEDLINE_BOLD[MB_LEN_MAX];
static char CROSS_POINT_RULEDLINE_BOLD_V[MB_LEN_MAX];
static char CROSS_POINT_RULEDLINE_BOLD_H[MB_LEN_MAX];
static char FULLWIDTH_NUMBERS[10][MB_LEN_MAX];
static char FULLWIDTH_SPACE[MB_LEN_MAX];

static void init_multibyte_char(void)
{
    setlocale(LC_CTYPE, "");
    wctomb(LEFT_TOP_RULEDLINE    , L'┏');
    wctomb(LEFT_BOTTOM_RULEDLINE , L'┗');
    wctomb(RIGHT_TOP_RULEDLINE   , L'┓');
    wctomb(RIGHT_BOTTOM_RULEDLINE, L'┛');
    wctomb(LEFT_SIDE_RULEDLINE   , L'┠');
    wctomb(LEFT_SIDE_RULEDLINE_BOLD   , L'┣');
    wctomb(RIGHT_SIDE_RULEDLINE  , L'┨');
    wctomb(RIGHT_SIDE_RULEDLINE_BOLD  , L'┫');
    wctomb(TOP_SIDE_RULEDLINE    , L'┯');
    wctomb(TOP_SIDE_RULEDLINE_BOLD    , L'┳');
    wctomb(BOTTOM_SIDE_RULEDLINE , L'┷');
    wctomb(BOTTOM_SIDE_RULEDLINE_BOLD , L'┻');
    wctomb(VERTICAL_RULEDLINE    , L'│');
    wctomb(VERTICAL_RULEDLINE_BOLD    , L'┃');
    wctomb(HORIZONTAL_RULEDLINE  , L'─');
    wctomb(HORIZONTAL_RULEDLINE_BOLD  , L'━');
    wctomb(CROSS_POINT_RULEDLINE , L'┼');
    wctomb(CROSS_POINT_RULEDLINE_BOLD , L'╋');
    wctomb(CROSS_POINT_RULEDLINE_BOLD_V , L'╂');
    wctomb(CROSS_POINT_RULEDLINE_BOLD_H , L'┿');
    wctomb(FULLWIDTH_NUMBERS[0]  , L'０');
    wctomb(FULLWIDTH_NUMBERS[1]  , L'１');
    wctomb(FULLWIDTH_NUMBERS[2]  , L'２');
    wctomb(FULLWIDTH_NUMBERS[3]  , L'３');
    wctomb(FULLWIDTH_NUMBERS[4]  , L'４');
    wctomb(FULLWIDTH_NUMBERS[5]  , L'５');
    wctomb(FULLWIDTH_NUMBERS[6]  , L'６');
    wctomb(FULLWIDTH_NUMBERS[7]  , L'７');
    wctomb(FULLWIDTH_NUMBERS[8]  , L'８');
    wctomb(FULLWIDTH_NUMBERS[9]  , L'９');
    wctomb(FULLWIDTH_SPACE, L'　');
}
int print_board(NUMPLEBOARD board)
{
    static int is_initialized = 0;
    char top_line[ROW_LEN_MAX];
    char bottom_line[ROW_LEN_MAX];
    char h_line[ROW_LEN_MAX];
    char h_line_bold[ROW_LEN_MAX];
    char numbers_line[ROW_LEN_MAX];
    int i,j,x,y,index;

    if( !is_initialized ){
        init_multibyte_char();
        is_initialized = 1;
    }

    *top_line = *bottom_line = *h_line = *h_line_bold = *numbers_line = '\0';
    strcat( top_line, LEFT_TOP_RULEDLINE );
    strcat( h_line, LEFT_SIDE_RULEDLINE );
    strcat( h_line_bold, LEFT_SIDE_RULEDLINE_BOLD );
    strcat( bottom_line, LEFT_BOTTOM_RULEDLINE );
    for(j = 0; j < 17; j++){
        if( j % 6 == 5){
            strcat( top_line, TOP_SIDE_RULEDLINE_BOLD);
            strcat( h_line, CROSS_POINT_RULEDLINE_BOLD_V );
            strcat( h_line_bold, CROSS_POINT_RULEDLINE_BOLD );
            strcat( bottom_line, BOTTOM_SIDE_RULEDLINE_BOLD );
        }else if( j % 2 == 0){
            strcat( top_line, HORIZONTAL_RULEDLINE_BOLD );
            strcat( h_line, HORIZONTAL_RULEDLINE );
            strcat( h_line_bold, HORIZONTAL_RULEDLINE_BOLD );
            strcat( bottom_line, HORIZONTAL_RULEDLINE_BOLD );
        }else{
            strcat( top_line, TOP_SIDE_RULEDLINE);
            strcat( h_line, CROSS_POINT_RULEDLINE );
            strcat( h_line_bold, CROSS_POINT_RULEDLINE_BOLD_H );
            strcat( bottom_line, BOTTOM_SIDE_RULEDLINE );
        }
    }
    strcat( top_line, RIGHT_TOP_RULEDLINE );
    strcat( h_line, RIGHT_SIDE_RULEDLINE );
    strcat( h_line_bold, RIGHT_SIDE_RULEDLINE_BOLD );
    strcat( bottom_line, RIGHT_BOTTOM_RULEDLINE );

    puts( top_line );
    for(i = 0; i < 17; i++){
        if( i % 2 == 0){
            *numbers_line = '\0';
            strcat( numbers_line, VERTICAL_RULEDLINE_BOLD );
            for(j = 0; j < 17; j++){
                if( j % 6 == 5){
                    strcat( numbers_line, VERTICAL_RULEDLINE_BOLD );
                }else if( j % 2 == 0){
                    x = i / 2;
                    y = j / 2;
                    index = board[x][y];
                    if( index == 0 ){
                        strcat( numbers_line, FULLWIDTH_SPACE );
                    }else{
                        strcat( numbers_line, FULLWIDTH_NUMBERS[index] );
                    }
                }else{
                    strcat( numbers_line, VERTICAL_RULEDLINE );
                }
            }
            strcat( numbers_line, VERTICAL_RULEDLINE_BOLD );
            puts( numbers_line );
        }else{
            if( i % 6 == 5 ){
                puts( h_line_bold );
            }else{
                puts( h_line );
            }
        }
    }
    puts( bottom_line );
    return 0;
}
/*
 * 座標(x,y)に数字が置けるかをチェック
 *
 * 戻り値: 1:置ける 0:置けない
 */
int check_place(NUMPLEBOARD board, int n, int x, int y)
{
    int i;
    int x0,y0;

    if( board[x][y] > 0 ){
        return 0;
    }
    x0 = (x / 3) * 3;
    y0 = (y / 3) * 3;
    for(i = 0; i < MAX_NUM; i++){
        /* horizontal line */
        if(board[x][i] == n){
            return 0;
        }
        /* vertical line */
        if(board[i][y] == n){
            return 0;
        }
        /* block */
        if(board[x0 + (i/3)][y0 + (i%3)] == n){
            return 0;
        }
    }
    return 1;
}
int put_place(NUMPLEBOARD board, int n, int x, int y)
{
    board[x][y] = n;
    return 1;
}
int del_place(NUMPLEBOARD board, int x, int y)
{
    board[x][y] = 0;
    return 1;
}
