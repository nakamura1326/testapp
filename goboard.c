/* 碁盤 */
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>

#define BOARD_WIDTH 21
#define BOARD_HEIGHT 21
#define BOARD_SIZE 441
#define BOARD(x,y) goboard[(y)*BOARD_WIDTH+(x)]
#define INDEX(x,y) ((y)*BOARD_WIDTH+(x))
#define INDEX_RX(i) (i%BOARD_WIDTH)
#define INDEX_RY(i) (i/BOARD_WIDTH)
#define SAMECOLOR(x,y,color) ((BOARD(x,y)&3)==color)

/* enum */
enum{
    NONE=0,
    BLACK=1,
    WHITE=3,
    OUT=2,
};
enum{
    D_NONE=0,
    D_LEFT=1,
    D_RIGHT=2,
    D_TOP=4,
    D_BOTTOM=8,
};

/* global variables */
int goboard[BOARD_SIZE];

/* function declaration */
void init_board(void);
void print_board(void);
int count_dame(int _x, int _y, int color);

int main(void)
{
    int i,j;

    init_board();
    for(i=1;i<BOARD_HEIGHT-1;i++){
        for(j=1;j<BOARD_WIDTH-1;j++){
            if(j%3==0){
                BOARD(j,i)=BLACK;
            }
        }
    }
    BOARD(2,1)=BLACK;
    BOARD(4,1)=BLACK;
    BOARD(5,1)=BLACK;
    BOARD(10,1)=BLACK;
    BOARD(11,1)=BLACK;
    BOARD(16,1)=BLACK;
    BOARD(17,1)=BLACK;
    BOARD(1,3)=BLACK;
    BOARD(1,6)=BLACK;
    BOARD(1,9)=BLACK;
    BOARD(1,12)=BLACK;
    BOARD(1,15)=BLACK;
    BOARD(1,18)=BLACK;
    BOARD(2,3)=BLACK;
    BOARD(2,6)=BLACK;
    BOARD(2,9)=BLACK;
    BOARD(2,12)=BLACK;
    BOARD(2,15)=BLACK;
    BOARD(2,18)=BLACK;
    BOARD(7,19)=BLACK;
    BOARD(8,19)=BLACK;
    BOARD(13,19)=BLACK;
    BOARD(14,19)=BLACK;

    print_board();
    i=count_dame(2,1,BLACK);
    /* 224 */
    printf("%d\n", i);

    init_board();
    BOARD(9,5)=BLACK;
    BOARD(10,5)=BLACK;
    BOARD(9,6)=BLACK;
    BOARD(11,6)=BLACK;
    BOARD(7,7)=BLACK;
    BOARD(8,7)=BLACK;
    BOARD(9,7)=BLACK;
    BOARD(10,7)=BLACK;
    BOARD(11,7)=BLACK;
    BOARD(7,8)=BLACK;
    BOARD(9,8)=BLACK;
    BOARD(10,8)=BLACK;
    BOARD(11,8)=BLACK;
    BOARD(7,9)=BLACK;
    BOARD(8,9)=BLACK;
    BOARD(9,9)=BLACK;
    BOARD(10,9)=BLACK;
    BOARD(11,9)=BLACK;

    BOARD(13,10)=BLACK;
    BOARD(14,10)=BLACK;
    BOARD(15,10)=BLACK;
    BOARD(16,10)=BLACK;
    BOARD(13,11)=BLACK;
    BOARD(12,12)=BLACK;
    BOARD(12,14)=BLACK;
    BOARD(13,14)=BLACK;
    BOARD(12,15)=BLACK;
    BOARD(14,11)=WHITE;
    BOARD(15,11)=WHITE;
    BOARD(16,11)=WHITE;
    BOARD(13,12)=WHITE;
    BOARD(14,12)=WHITE;
    BOARD(15,12)=WHITE;
    BOARD(16,12)=WHITE;
    BOARD(14,13)=WHITE;
    BOARD(17,13)=WHITE;
    BOARD(18,13)=WHITE;
    BOARD(14,14)=WHITE;
    BOARD(15,14)=WHITE;
    BOARD(16,14)=WHITE;
    BOARD(18,14)=WHITE;
    BOARD(19,14)=WHITE;
    BOARD(13,15)=WHITE;
    BOARD(16,15)=WHITE;
    BOARD(17,15)=WHITE;
    BOARD(19,15)=WHITE;
    BOARD(11,16)=WHITE;
    BOARD(12,16)=WHITE;
    BOARD(13,16)=WHITE;
    BOARD(11,17)=WHITE;
    BOARD(13,17)=WHITE;
    BOARD(13,18)=WHITE;
    BOARD(13,19)=WHITE;



    print_board();
    i=count_dame(10,8,BLACK);
    /* 20 */
    printf("%d\n", i);
    i=count_dame(15,12,WHITE);
    /* 11 */
    printf("%d\n", i);

    return 0;
}
void init_board(void)
{
    int i,a,r;
    for(i=0;i<BOARD_SIZE;i++){
        a = i / BOARD_WIDTH;
        r = i % BOARD_WIDTH;
        if(a == 0 || a == BOARD_HEIGHT - 1){
            goboard[i]=OUT;
        }else if(r == 0 || r == BOARD_WIDTH-1){
            goboard[i]=OUT;
        }else{
            goboard[i]=NONE;
        }
    }
}
void print_board(void)
{
    wchar_t left_top_corner = 0x250c;
    wchar_t left_bottom_corner = 0x2514;
    wchar_t right_top_corner = 0x2510;
    wchar_t right_bottom_corner = 0x2518;
    wchar_t left_side = 0x251c;
    wchar_t right_side = 0x2524;
    wchar_t top_side = 0x252c;
    wchar_t bottom_side = 0x2534;
    wchar_t cross_point = 0x253c;
    wchar_t star_point = 0x254b;
    wchar_t black_stone = 0x25cf;
    wchar_t white_stone = 0x25cb;
    wchar_t visual_board[22][24];
    wchar_t (*vbp)[24];
    const wchar_t *vbpp;
    int l_offset, r_offset, t_offset, b_offset;
    int brd_sz = 19;
    int i, j;
#define M_V_BOARD_SZ 2048
    char m_visual_board[M_V_BOARD_SZ];
    mbstate_t ps;
    size_t len;

    l_offset = 2;
    r_offset = 2;
    t_offset = 1;
    b_offset = 1;
    memset(&ps,0,sizeof(mbstate_t));
    setlocale(LC_ALL, "");
    /*
    i = fwide(stdout, 0);
    printf("fwide:%d\n", i);
    */
    i = 0;
    for(j = l_offset; j < brd_sz + l_offset; j++){
        visual_board[0][j] = L'A' - l_offset + j;
    }
    visual_board[i][0] = L' ';
    visual_board[i][1] = L' ';
    visual_board[i][21]= L' ';
    visual_board[i][22]= L' ';
    visual_board[i][23]= L'\n';
    for(i = t_offset; i < t_offset+brd_sz; i++){
        visual_board[i][0] = L' ';
        visual_board[i][1] = L' ';
        for( j = l_offset; j < l_offset + brd_sz; j++){
            switch(BOARD(j-1,i)){
                case NONE:
                    if(i == t_offset){
                        if( j == l_offset ){
                            visual_board[i][j] = left_top_corner;
                        }else if(j == l_offset + brd_sz - 1){
                            visual_board[i][j] = right_top_corner;
                        }else{
                            visual_board[i][j] = top_side;
                        }
                    }else if(i == t_offset + brd_sz - 1){
                        if( j == l_offset ){
                            visual_board[i][j] = left_bottom_corner;
                        }else if(j == l_offset + brd_sz - 1){
                            visual_board[i][j] = right_bottom_corner;
                        }else{
                            visual_board[i][j] = bottom_side;
                        }
                    }else{
                        if( j == l_offset ){
                            visual_board[i][j] = left_side;
                        }else if(j == l_offset + brd_sz - 1){
                            visual_board[i][j] = right_side;
                        }else{
                            if( i == t_offset + 3
                                || i == t_offset + 9
                                || i == t_offset + 15){
                                if( j == l_offset + 3
                                    || j == l_offset + 9
                                    || j == l_offset + 15){
                                    visual_board[i][j] = star_point;
                                }else{
                                    visual_board[i][j] = cross_point;
                                }
                            }else{
                                visual_board[i][j] = cross_point;
                            }
                        }
                    }
                    break;
                case BLACK:
                    visual_board[i][j] = black_stone;
                    break;
                case WHITE:
                    visual_board[i][j] = white_stone;
                    break;
                case OUT:
                    break;

            }
        }
        visual_board[i][21]= L' ';
        visual_board[i][22]= L' ';
        visual_board[i][23]= L'\n';
    }
    for(j = l_offset; j < l_offset + brd_sz; j++){
        visual_board[i][j] = L'A' - l_offset + j;
    }
    visual_board[i][0] = L' ';
    visual_board[i][1] = L' ';
    visual_board[i][21]= L' ';
    visual_board[i][22]= L' ';
    visual_board[i][23]= L'\n';

    visual_board[21][0]= L'\0';

    vbp = visual_board;
    vbpp = *vbp;

    len = wcsrtombs(m_visual_board, &vbpp, M_V_BOARD_SZ, &ps);
#undef M_V_BOARD_SZ
    printf("%s", m_visual_board);
    
}
int count_dame(int _x, int _y, int color){
    int x, y, d, sp, dp, cp, idx, i;
    int stack[360] = {0};
    int dame[256] = {0};
    int checked[360] = {0};
    int cycle;
#define COUNT_DAME for(i=dp-1;i>=0;i--){\
    if(dame[i] == INDEX(x,y)){\
        break;\
    }\
}\
if(i<0){\
    dame[dp++]= INDEX(x,y);\
}
#define COUNT_CHECKED(drc) for(i=cp-1;i>=0;i--){\
    if(checked[i] == INDEX(x,y)){\
        break;\
    }\
}\
if(i<0){\
    checked[cp++]=INDEX(x,y);\
    stack[sp++]=idx<<0x04 | d;\
    d = (drc);\
    continue;\
}
    x = _x; y =_y; d = D_NONE; sp = 0; dp = 0; cp = 0; cycle = 0;
    while(1){
        cycle++;
        idx = INDEX(x,y);
        /* left */
        if((d & D_LEFT) != D_LEFT){
            d |= D_LEFT;
            x -= 1;
            if(BOARD(x,y) == NONE){
                COUNT_DAME
            }else if(SAMECOLOR(x,y,color)){
                COUNT_CHECKED(D_RIGHT)
            }
            x += 1;
        }
        /* right */
        if((d & D_RIGHT) != D_RIGHT){
            d |= D_RIGHT;
            x += 1;
            if(BOARD(x,y) == NONE){
                COUNT_DAME
            }else if(SAMECOLOR(x,y,color)){
                COUNT_CHECKED(D_LEFT)
            }
            x -= 1;
        }
        /* top */
        if((d & D_TOP) != D_TOP){
            d |= D_TOP;
            y -= 1;
            if(BOARD(x,y) == NONE){
                COUNT_DAME
            }else if(SAMECOLOR(x,y,color)){
                COUNT_CHECKED(D_BOTTOM)
            }
            y += 1;
        }
        /* bottom */
        if((d & D_BOTTOM) != D_BOTTOM){
            d |= D_BOTTOM;
            y += 1;
            if(BOARD(x,y) == NONE){
                COUNT_DAME
            }else if(SAMECOLOR(x,y,color)){
                COUNT_CHECKED(D_TOP)
            }
            y -= 1;
        }
        if(sp == 0){
            break;
        }
        idx = stack[--sp];
        d = idx & 0x0f;
        idx >>= 0x04;
        x = INDEX_RX(idx);
        y = INDEX_RY(idx);
    }
    return dp;
}
    
