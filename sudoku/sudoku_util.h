/* NUMBER PLACE (数独) */
#ifndef __SUDOKU_UTIL_H__
#define __SUDOKU_UTIL_H__

#define MAX_NUM 9
#define ROW_LEN_MAX 256
#define GET_BLOCKID_SUDOKU_UTIL(x,y) (((x)/3)*3+(y)/3)
#define GET_BLOCK_X_SUDOKU_UTIL(x,i) (((x)/3)*3+(i/3))
#define GET_BLOCK_Y_SUDOKU_UTIL(Y,i) (((y)/3)*3+(i%3))
#define GET_BLOCK_X_BYID_SUDOKU_UTIL(id,offset) (((id)/3)*3+((offset)/3))
#define GET_BLOCK_Y_BYID_SUDOKU_UTIL(id,offset) (((id)%3)*3+((offset)%3))
#define SWAP_SUDOKU_UTIL(type,a,b) { type temp = a; a = b; b = temp; }

enum{
    NUMPLE_GROUP_HORIZONTAL_LINE,
    NUMPLE_GROUP_VERTICAL_LINE,
    NUMPLE_GROUP_BLOCK,
};

typedef int(*NUMPLEBOARD)[MAX_NUM];

int print_board(NUMPLEBOARD board);
int check_place(NUMPLEBOARD board, int n, int x, int y);
int put_place(NUMPLEBOARD board, int n, int x, int y);
int del_place(NUMPLEBOARD board, int x, int y);
#endif /* __SUDOKU_UTIL_H__ */
