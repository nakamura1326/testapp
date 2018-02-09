/* NUMBER PLACE (数独) */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sudoku.h"

typedef struct{
    int n;
    int block;
    int x;
    int y;
} cell_info_t;

/*
 * 正の整数 n を超えない(less than n) 0以上の整数をランダムに返却する
 */
int get_rand(int n)
{
    srand((unsigned)time(NULL));
    return rand() % n;
}


/*
 *ブロック単位で置く事のできる座標を返す
 * pt_list: 置く事のできる座標のリスト。呼び出し元への返却用
 * 返り値:  置く事のできる座標の個数
 */
int check_at_block(NUMPLEBOARD board, int n, int block_n, cell_info_t *pt_list)
{
    /*
     * (x0, y0):ブロックの左上の座標
     * (x1, y1):ブロックの右下の座標
     */
    int x0,y0,x1,y1;
    int x,y;
    int c = 0;

    x0 = ( block_n / 3 ) * 3;
    x1 = x0 + 2;
    y0 = ( block_n % 3 ) * 3;
    y1 = y0 + 2;
    //printf("block[%d] p0(%d,%d),p1(%d,%d)\n", block_n, x0,y0,x1,y1);

    for(x = x0; x <= x1; x++){
        for(y = y0; y <= y1; y++){
            if( check_place( board, n, x, y )){
                pt_list->x = x;
                pt_list->y = y;
                pt_list->n = n;
                pt_list->block = block_n;
                pt_list++;
                c++;
            }
        }
    }
    return c;
}
void putcell(NUMPLEBOARD board, cell_info_t *ci)
{
    board[ci->x][ci->y] = ci->n;
}
void delcell(NUMPLEBOARD board, cell_info_t *ci)
{
    board[ci->x][ci->y] = 0;
}
int create_numplace(NUMPLEBOARD board)
{   
    int result; /* 置く事のできるCellの数 */
    int i,j,k;  /* ループカウンタ */
    int r;  /* 乱数 */
    cell_info_t stack[1024]; /* 置く事のできるCellのうち使わない候補を予備として積んでおく */
    cell_info_t *cp;    /* Cell Stackのポインタ */
    cell_info_t history_list[100]; /* 置いてきたCellの履歴 */
    cell_info_t *hp;    /* Historyのポインタ */
    cell_info_t ci;     /* 現在のCell */
    cell_info_t tmp_list[MAX_NUM]; /* 置く事のできるCellの情報を一時的に格納 */

    hp = history_list;
    hp->n = 0;  /* dummy */
    cp = stack;
    for(i = 1; i <= MAX_NUM; i++){    /* 各数字について */
        for(j = 0; j < MAX_NUM; j++){ /* 各ブロックについて */
            result = check_at_block(board, i, j, tmp_list);
            if( result == 0 ){
                /* どこにも置けないため、現在の数字を取り消し
                 * スタックに保存しておいた過去の候補に置き換える
                 */
                ci = *--cp;
                i = ci.n;
                j = ci.block;
                do{
                    /* スタックからPOPしたCell情報に基づいて
                     * 数字を置ける盤面になるまで履歴をさかのぼる
                     */
                    delcell(board, hp);
                    if( hp->n == ci.n && hp->block == ci.block ){
                        putcell(board, &ci);
                        *hp = ci;
                        break;
                    }
                    hp--;
                }while(hp->n > 0);
            }else if( result >= 2 ){
                /* 数字を置ける場所が複数候補ある場合は
                 * ランダムに1つ取り出し数字を置く
                 * 残りはスタックに積む
                 */
                r = get_rand( result );
                for(k = 0; k < result; k++){
                    if( k == r){
                        ci = tmp_list[r];
                        putcell(board, &ci);
                        *++hp = ci;
                    }else{
                        *cp++ = tmp_list[k];
                    }
                }
            }else{
                ci = tmp_list[0];
                putcell(board, &ci);
                *++hp = ci;
            }
        }
    }
    return 1;
}

int main(void)
{
    int board[MAX_NUM][MAX_NUM] = {
        /*
        {0,0,0,0,1,0,7,0,0},
        {7,0,0,0,8,0,0,0,0},
        {9,0,0,4,0,0,0,0,0},
        {0,9,2,0,3,0,0,6,7},
        {3,6,0,0,2,1,5,9,4},
        {0,5,0,0,0,4,0,8,0},
        {6,7,3,1,0,9,4,0,8},
        {2,4,9,8,7,3,6,1,5},
        {8,1,5,2,0,6,3,0,0},
        */
        {0,0,0,3,0,0,0,5,0},
        {0,8,1,0,2,0,7,0,0},
        {0,0,0,0,0,6,0,0,4},
        {0,0,0,4,0,0,0,0,0},
        {0,0,0,0,3,5,0,7,0},
        {6,4,9,0,0,0,0,0,0},
        {9,2,0,0,0,0,0,6,0},
        {0,0,0,8,0,0,0,0,0},
        {0,7,0,1,0,3,0,0,8},
    };
    /*
    create_numplace(board);
    */
    print_board(board);
    if( solve_numplace(board)){
        puts("clear!");
    }else{
        puts("give up..");
    }
    print_board(board);
    return 0;
}
