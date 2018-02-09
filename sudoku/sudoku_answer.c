/* 数独を解く */

#include <stdio.h>
#include <string.h>
#include "sudoku.h"

#define NUMPLACE_ON 1
#define NUMPLACE_OFF 0

typedef struct{
    int num;
    int block_id;
    int inblock_id;
    int flags[MAX_NUM];
}place_info_t;

typedef place_info_t (*PLACEINFO_TABLE)[MAX_NUM];
/* 関数プロトタイプ宣言 function declaration */
int check_puttable_num(NUMPLEBOARD board, int x, int y);
void check_puttable_num2(NUMPLEBOARD board, int x, int y, int *flags);
static void set_matrix( int (*matrix)[MAX_NUM], PLACEINFO_TABLE ptable, int group, int id);
static void print_matrix( int (*matrix)[MAX_NUM], int group, int id);
static void merge_matrix( int (*matrix)[MAX_NUM], PLACEINFO_TABLE ptable, int group, int id);

int print_place_info_atpos( PLACEINFO_TABLE ptable, char *str, int x, int y, int n_line)
{
    char *sp;
    char chars[MAX_NUM];
    char bignums[3][9][4] = {
        {"   "," _ "," _ ","   "," _ "," _ "," _ "," _ "," _ "},
        {" | "," _|"," _|","|_|","|_ ","|_ ","| |","|_|","|_|"},
        {" | ","|_ "," _|","  |"," _|","|_|","  |","|_|"," _|"},
    };
    int n, i;

    if( ptable[x][y].num == 0 ){
        for( i = n_line * 3; i < n_line * 3 + 3; i++){
            chars[i] = ptable[x][y].flags[i] == NUMPLACE_ON
                ? (i + 1) + '0'
                : '.';
        }
        n = sprintf( str, " %c%c%c |"
                , chars[ n_line * 3 ]
                , chars[ n_line * 3 + 1]
                , chars[ n_line * 3 + 2]
               );
    }else{
        n = sprintf( str, " %s |", bignums[n_line][ptable[x][y].num-1]);
    }
    return n;
}
void print_place_info_table2_atgroup( PLACEINFO_TABLE ptable, int group, int id )
{
    char str[1024];
    char *sp;

    int n_line;
    int n;
    int i,j,x,y;

    if( group == NUMPLE_GROUP_HORIZONTAL_LINE ){
        puts( "-------------------------------------------------------" );
        sp = str;
        for( n_line = 0; n_line < 3; n_line++){
            *sp++ = '|';
            for( i = 0; i < MAX_NUM; i++){
                n = print_place_info_atpos( ptable, sp, id, i, n_line );
                sp += n;
            }
            *sp++ = '\n';
        }
        *sp = '\0';
        strcat( sp, "-------------------------------------------------------\n");
        sp = strrchr( sp, '\0');
    }else if( group == NUMPLE_GROUP_VERTICAL_LINE ){
        puts( "------" );
        sp = str;
        for( i = 0; i < MAX_NUM; i++ ){
            for( n_line = 0; n_line < 3; n_line++){
                n = print_place_info_atpos( ptable, sp, i, id, n_line );
                sp += n;
                *sp++ = '\n';
            }
            *sp = '\0';
            strcat( sp, "------\n");
            sp = strrchr( sp, '\0');
        }
    }else if( group == NUMPLE_GROUP_BLOCK ){
        puts( "------------------" );
        sp = str;
        for( i = 0; i < 3; i++){
            for( n_line = 0; n_line < 3; n_line++){
                for( j = 0; j < 3; j++){
                    x = GET_BLOCK_X_BYID_SUDOKU_UTIL(id, i*3+j);
                    y = GET_BLOCK_Y_BYID_SUDOKU_UTIL(id, i*3+j);
                    n = print_place_info_atpos( ptable, sp, x, y, n_line );
                    sp += n;
                }
                *sp++ = '\n';
            }
            *sp = '\0';
            strcat( sp, "------------------\n");
            sp = strrchr( sp, '\0');
        }
    }
    *sp = '\0';
    printf( str );
}
void print_place_info_table2( PLACEINFO_TABLE ptable )
{
    char str[1024];
    char *sp;

    int n_line;
    int n;
    int num;
    int i,j;

    for(i = 0; i < MAX_NUM; i++){
        if( i % 3 == 0 ){
            puts( "========================================================" );
        }else{
            puts( "--------------------------------------------------------" );
        }
        sp = str;
        for(n_line = 0; n_line < 3; n_line++){
            for(j = 0; j < MAX_NUM; j++){
                num = ptable[i][j].num;
                if( j % 3 == 0 ){
                    *sp++ = '|';
                }
                n = print_place_info_atpos( ptable, sp, i, j, n_line);
                sp += n;
            }
            *sp++ = '\n';
        }
        *sp = '\0';
        printf(str);
    }
}
int set_place_info_table( PLACEINFO_TABLE ptable, NUMPLEBOARD board )
{
    int i,j,k;
    for(i = 0; i < MAX_NUM; i++){
        for(j = 0; j < MAX_NUM; j++){
            ptable[i][j].num = board[i][j];
            ptable[i][j].block_id = (i/3)*3 + (j/3);
            ptable[i][j].inblock_id = (i%3)*3 + (j%3);
            for(k = 0; k < MAX_NUM; k++){
                ptable[i][j].flags[k] = NUMPLACE_OFF;
            }
            if( board[i][j] == 0 ){
                check_puttable_num2(board,i,j,ptable[i][j].flags);
            }
        }
    }
    return 1;
}

place_info_t *get_place_info( PLACEINFO_TABLE ptable, int group, int id, int offset)
{
    int x,y;
    switch(group){
        case NUMPLE_GROUP_HORIZONTAL_LINE:
            x = id;
            y = offset;
            break;
        case NUMPLE_GROUP_VERTICAL_LINE:
            x = offset;
            y = id;
            break;
        case NUMPLE_GROUP_BLOCK:
            x = GET_BLOCK_X_BYID_SUDOKU_UTIL( id, offset );
            y = GET_BLOCK_Y_BYID_SUDOKU_UTIL( id, offset );
            break;
    }
    return &ptable[x][y];
}
int update_place_info_atgroup( PLACEINFO_TABLE ptable, int group, int id ){
    /* 
     * firstに最初に見つかったマスの
     * グループ内オフセットを保持しておく
     * (見つからなかった場合,もしくは2つ見つかった以上の場合は'-1')
     */
    int first;
    int matrix[MAX_NUM][MAX_NUM] = {};
    int matrix_row[MAX_NUM][MAX_NUM] = {};
    int matrix_col[MAX_NUM][MAX_NUM] = {};
    int matrix_blk[MAX_NUM][MAX_NUM] = {};
    /* 配列の添え字はマスのindex, 配列の値は[数字] */
    int updatetargets[MAX_NUM] = { -1,-1,-1,-1,-1,-1,-1,-1,-1 };
    char *s_group;
    place_info_t *pi_ptr;
    int i,j,count,n,pos,x,y;
    int row_id, col_id, blk_id;

    set_matrix( matrix, ptable, group, id );
    /* 行ごと */
    for(j = 0; j < MAX_NUM; j++){
        first = -1;
        for( i = 0; i < MAX_NUM; i++){
            if( matrix[j][i] == NUMPLACE_ON ){
                if( first == -1 ){
                    /* 最初の1コ発見 */
                    first = i;
                }else{
                    /* 2コ目が見つかったらアウト */
                    first = -1;
                    break;
                }
            }
        }
        if(first != -1 ){
            updatetargets[first] = j;
        }
    }
    /* 列ごと */
    for(i = 0; i < MAX_NUM; i++){
        first = -1;
        for(j = 0; j < MAX_NUM; j++){
            if(matrix[j][i] == NUMPLACE_ON ){
                if( first == -1 ){
                    /* 最初の1コ発見 */
                    first = j;
                }else{
                    /* 2コ目が見つかったらアウト */
                    first = -1;
                    break;
                }
            }
        }
        if( first != -1 ){
            updatetargets[i] = first;
        }
    }
    count = 0;
    for(i = 0; i < MAX_NUM; i++){
        if( updatetargets[i] != -1 ){
            count++;
        }
    }
    if(count == 0){
        return count;
    }
    print_matrix( matrix, group, id );
    switch( group ){
        case NUMPLE_GROUP_HORIZONTAL_LINE:
            s_group = "Horizontal";
            break;
        case NUMPLE_GROUP_VERTICAL_LINE:
            s_group = "Vertical";
            break;
        case NUMPLE_GROUP_BLOCK:
            s_group = "Block";
            break;
    }
    for(i = 0; i < MAX_NUM; i++){
        n = updatetargets[i];
        if( n != -1){
            pos = i;
            pi_ptr = get_place_info( ptable, group, id, pos );
            pi_ptr->num = n + 1;
            for(j = 0; j < MAX_NUM; j++){
                pi_ptr->flags[j] = NUMPLACE_OFF;
            }
                
            printf("UpdatePlaceInfo (%10s, id:%d, pos:%d, num:%d)\n\n\n", s_group, id, pos, n + 1);
            /* 影響範囲の候補を更新する */
            switch( group ){
                case NUMPLE_GROUP_HORIZONTAL_LINE:
                    row_id = id;
                    col_id = pos;
                    blk_id = GET_BLOCKID_SUDOKU_UTIL(id, pos );
                    break;
                case NUMPLE_GROUP_VERTICAL_LINE:
                    row_id = pos;
                    col_id = id;
                    blk_id = GET_BLOCKID_SUDOKU_UTIL(pos, id );
                    break;
                case NUMPLE_GROUP_BLOCK:
                    x = GET_BLOCK_X_BYID_SUDOKU_UTIL(id, pos);
                    y = GET_BLOCK_Y_BYID_SUDOKU_UTIL(id, pos);
                    row_id = x;
                    col_id = y;
                    blk_id = id;
                    break;
            }
            set_matrix( matrix_row, ptable, NUMPLE_GROUP_HORIZONTAL_LINE, row_id);
            set_matrix( matrix_col, ptable, NUMPLE_GROUP_VERTICAL_LINE, col_id);
            set_matrix( matrix_blk, ptable, NUMPLE_GROUP_BLOCK, blk_id);
            for(j = 0; j < MAX_NUM; j++){
                matrix_row[n][j] = NUMPLACE_OFF;
                matrix_col[n][j] = NUMPLACE_OFF;
                matrix_blk[n][j] = NUMPLACE_OFF;
            }
            merge_matrix( matrix_row, ptable, NUMPLE_GROUP_HORIZONTAL_LINE, row_id);
            merge_matrix( matrix_col, ptable, NUMPLE_GROUP_VERTICAL_LINE, col_id);
            merge_matrix( matrix_blk, ptable, NUMPLE_GROUP_BLOCK, blk_id);
        }
    }
    return count;
}
void update_place_info( PLACEINFO_TABLE ptable ){
    int count,i;

    do{
        count = 0;
        for( i = 0; i < MAX_NUM ; i++){
            count += update_place_info_atgroup( ptable, NUMPLE_GROUP_HORIZONTAL_LINE, i);
            count += update_place_info_atgroup( ptable, NUMPLE_GROUP_VERTICAL_LINE, i);
            count += update_place_info_atgroup( ptable, NUMPLE_GROUP_BLOCK, i);
        }
        printf("update count = %d \n\n", count);
        print_place_info_table2( ptable );
    }while( count > 0);
    //}while(0);
}

void feedback_to_board( PLACEINFO_TABLE ptable, NUMPLEBOARD board)
{
    int i,j;
    for(i = 0; i < MAX_NUM; i++)
        for(j = 0; j < MAX_NUM; j++)
            board[i][j] = ptable[i][j].num;
}

void print_place_info_table( PLACEINFO_TABLE ptable )
{
    int i,j,k;
    for(i = 0; i < MAX_NUM; i++){
        for(j = 0; j < MAX_NUM; j++){
            if( ptable[i][j].num != 0){
                continue;
            }
            printf("block(%d,%d) (x:%d,y:%d) "
                    , ptable[i][j].block_id
                    , ptable[i][j].inblock_id
                    , i
                    , j
                    );
            for(k = 0; k < MAX_NUM; k++){
                if( ptable[i][j].flags[k] == NUMPLACE_ON ){
                    printf("%d,", k+1);
                }
            }
            putchar('\n');
        }
    }
}

void print_place_info_table_atblock( PLACEINFO_TABLE ptable, int block_id)
{
    int i,j,k;
    for(i = 0; i < MAX_NUM; i++){
        for(j = 0; j < MAX_NUM; j++){
            if( ptable[i][j].num != 0){
                continue;
            }
            if( ptable[i][j].block_id != block_id ){
                continue;
            }
            printf("block(%d,%d) (x:%d,y:%d) "
                    , ptable[i][j].block_id
                    , ptable[i][j].inblock_id
                    , i
                    , j
                    );
            for(k = 0; k < MAX_NUM; k++){
                if( ptable[i][j].flags[k] == NUMPLACE_ON ){
                    printf("%d,", k+1);
                }
            }
            putchar('\n');
        }
    }
}

int get_space_count(NUMPLEBOARD board){
    int i,j;
    int c = 0;
    for(i = 0; i < MAX_NUM; i++){
        for(j = 0; j < MAX_NUM; j++){
            if(board[i][j] == 0){
                c++;
            }
        }
    }
    return c;
}
int search_hidden_one(NUMPLEBOARD board){
    int space_count;
    int space_count_init = 0;
    int put_count;
    int i,j,k,l;
    int flag;

    space_count_init = get_space_count(board);
    printf("space_count_init:%d\n", space_count_init);
    space_count = space_count_init;
    do{
        put_count = 0;
        for(i = 0; i < MAX_NUM; i++){
            for(j = 0; j < MAX_NUM; j++){
                if(board[i][j] == 0){
                    flag = check_puttable_num(board, i, j);
                    /* 置ける数字が1つだけ
                     * (=bitが1つだけ立っている状態)の場合 */
                    if(flag && (flag & flag - 1) == 0){
                        for(l = 0; l < MAX_NUM; l++){
                            /* 立っているbitのケタから数字を算出 */
                            if(flag >> l == 1){
                                put_count++;
                                put_place(board, l+1, i, j);
                                printf("%d (%d,%d)\n", l+1,i,j);
                                //print_board(board);
                            }
                        }
                    }
                }
            }
        }
        if(put_count == 0){
            return 0;
        }else{
            space_count -= put_count;
        }
    }while(space_count > 0);
    return 1;
}
int search_line_block(NUMPLEBOARD board)
{
    struct search_info{
        int count;
        int x;
        int y;
    } si_hline, si_vline, si_block;
    int i,j,n,x0,y0,x,y;
    int space_count,space_count_init,put_count;

    space_count_init = get_space_count(board);
    printf("space_count_init:%d\n", space_count_init);
    space_count = space_count_init;
    do{
        put_count = 0;
        for(i = 0; i < MAX_NUM; i++){
            for(n = 1; n <= MAX_NUM; n++){
                /* for each horizontal line */
                si_hline.count = 0;
                for(j = 0; j < MAX_NUM; j++){
                    if( board[i][j] == n ){
                        si_hline.count = 0;
                        break;
                    }else if( board[i][j] == 0 && check_place(board,n,i,j)){
                        si_hline.count++;
                        si_hline.y = j;
                    }
                }
                if( si_hline.count == 1 ){
                    put_place(board,n,i,si_hline.y);
                    put_count++;
                    printf("h_line %d (%d,%d)\n", n,i,si_hline.y);
                }
                /* for each vertical line */
                si_vline.count = 0;
                for(j = 0; j < MAX_NUM; j++){
                    if( board[j][i] == n ){
                        si_vline.count = 0;
                        break;
                    }else if( board[j][i] == 0 && check_place(board,n,j,i)){
                        si_vline.count++;
                        si_vline.x = j;
                    }
                }
                if( si_vline.count == 1 ){
                    put_place(board,n,si_vline.x,i);
                    put_count++;
                    printf("v_line %d (%d,%d)\n", n,si_vline.x,i);
                }
                /* for each block */
                x0 = (i / 3) * 3;
                y0 = (i % 3) * 3;
                for(j = 0; j < MAX_NUM; j++){
                    x = x0 + (j/3);
                    y = y0 + (j%3);
                    if( board[x][y] == n ){
                        si_block.count = 0;
                        break;
                    }else if( board[x][y] == 0 && check_place(board,n,x,y)){
                        si_block.count++;
                        si_block.x = x;
                        si_block.y = y;
                    }
                }
                if( si_block.count == 1 ){
                    put_place(board,n,si_block.x,si_block.y);
                    put_count++;
                    printf(" block %d (%d,%d)\n", n,si_block.x,si_block.y);
                }
            }
        }
        if(put_count == 0){
            return 0;
        }else{
            space_count -= put_count;
        }
    }while(space_count > 0);
    return 1;
}
static void set_matrix( int (*matrix)[MAX_NUM], PLACEINFO_TABLE ptable, int group, int id)
{
    int x,y,j,k;
    for( j = 0; j < MAX_NUM; j++){
        switch(group){
            case NUMPLE_GROUP_HORIZONTAL_LINE:
                x = id;
                y = j;
                break;
            case NUMPLE_GROUP_VERTICAL_LINE:
                x = j;
                y = id;
                break;
            case NUMPLE_GROUP_BLOCK:
                x = GET_BLOCK_X_BYID_SUDOKU_UTIL(id,j);
                y = GET_BLOCK_Y_BYID_SUDOKU_UTIL(id,j);
                break;
        }
        for( k = 0; k < MAX_NUM; k++ ){
            matrix[k][j] = ptable[x][y].flags[k];
        }
    }
}
static void print_matrix( int (*matrix)[MAX_NUM], int group, int id)
{
    int c, i, j;
    int count_col[MAX_NUM] = {0};

    switch(group){
        case NUMPLE_GROUP_HORIZONTAL_LINE:
            printf("Horizontal line No.%d\n", id);
            break;
        case NUMPLE_GROUP_VERTICAL_LINE:
            printf("  Vertical line No.%d\n", id);
            break;
        case NUMPLE_GROUP_BLOCK:
            printf("          Block No.%d\n", id);
            break;
    }

    puts("num/index|0 1 2 3 4 5 6 7 8 |count_row");
    for(i = 0; i < MAX_NUM; i++){
        c = 0;
        printf(" %d       |", i + 1 );
        for( j = 0; j < MAX_NUM; j++){
            if( matrix[i][j] == NUMPLACE_ON ){
                putchar('*');
                c++;
            }else if( matrix[i][j] == NUMPLACE_OFF){
                putchar(' ');
            }
            putchar(' ');

            if( matrix[j][i] == NUMPLACE_ON ){
                count_col[i]++;
            }
        }
        printf("| %d\n", c);
    }
    printf("count_col|");
    for(i = 0; i < MAX_NUM; i++){
        printf("%d ", count_col[i]);
    }
    putchar('\n');
    putchar('\n');
}
static void merge_matrix( int (*matrix)[MAX_NUM], PLACEINFO_TABLE ptable, int group, int id)
{
    int i,j,x,y;
    for(i = 0; i < MAX_NUM; i++){
        switch( group ){
            case NUMPLE_GROUP_HORIZONTAL_LINE:
                x = id;
                y = i;
                break;
            case NUMPLE_GROUP_VERTICAL_LINE:
                x = i;
                y = id;
                break;
            case NUMPLE_GROUP_BLOCK:
                x = GET_BLOCK_X_BYID_SUDOKU_UTIL( id, i );
                y = GET_BLOCK_Y_BYID_SUDOKU_UTIL( id, i );
                break;
        }
        for(j = 0; j < MAX_NUM; j++){
            ptable[x][y].flags[j] = matrix[j][i];
        }
    }
}
void search_nakid_pair_atgroup(PLACEINFO_TABLE ptable, int grouptype, int id)
{
   struct countinfo{
        int count;
        int numbers[MAX_NUM];
    };
    struct pairinfo{
        /* number 1,2 */
        int n1;
        int n2;
        /* index 1,2 */
        int i1;
        int i2;
        int count;
    };

    /*
     *num/index|0 1 2 3 4 5 6 7 8 |count_row
     * 1       |                  | 0
     * 2       |                  | 0
     * 3       |                  | 0
     * 4       |  * *   *     *   | 4
     * 5       |* *     *     *   | 4
     * 6       |        *     *   | 2 -- 
     * 7       |* * *   *         | 4  |
     * 8       |                  | 0  |
     * 9       |        *     *   | 2 --->pair
     *count_col|2 3 2 0 5 0 0 4 0
     *          |   |
     *          ------>not pair
     *
     * 置ける可能性のある数字(num)と場所(index)の組み合わせを表形式にする
     */
    /* TODO:初期化 */
    int matrix[MAX_NUM][MAX_NUM] = {
    };
    int matrix_tmp[MAX_NUM][MAX_NUM];
    struct countinfo ci_row[MAX_NUM] = {{0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, };
    struct countinfo ci_col[MAX_NUM] = {{0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, {0,{0}}, };
    struct pairinfo nakid_pairs[MAX_NUM] = {{0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},} ;
    struct pairinfo hidden_pairs[MAX_NUM]= {{0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},} ;;

    int j,k,l;
    int i1,i2;
    int row,col;
    
    set_matrix( matrix, ptable, grouptype, id );
    /*
    print_matrix( matrix, grouptype, id );
    */
    /* matrixの行ごとの和 */
    for( j = 0; j < MAX_NUM; j++){
        for( k = 0; k < MAX_NUM; k++) {
            if( matrix[j][k] == NUMPLACE_ON ){
                ci_row[j].numbers[ci_row[j].count] = k;
                ci_row[j].count++;
            }
        }
        if( ci_row[j].count == 2){
            for(l = 0; l < MAX_NUM; l++){
                if( hidden_pairs[l].count == 0 ){
                    hidden_pairs[l].n1 = j;
                    hidden_pairs[l].i1 = ci_row[j].numbers[0];
                    hidden_pairs[l].i2 = ci_row[j].numbers[1];
                    hidden_pairs[l].count++;
                    break;
                }else if( hidden_pairs[l].i1 == ci_row[j].numbers[0]
                       && hidden_pairs[l].i2 == ci_row[j].numbers[1]){
                    if( hidden_pairs[l].count == 1 ){
                        hidden_pairs[l].n2 = j;
                    }
                    hidden_pairs[l].count++;
                    break;
                }
            }
        }
            
    }
    /* ペアの抽出 */
    for( j = 0; j < MAX_NUM; j++){
        if( hidden_pairs[j].count == 2 ){
            printf("hidden_pair group:%d id:%d num(%d,%d) masu(%d,%d)\n"
                    , grouptype
                    , id
                    , hidden_pairs[j].n1 + 1
                    , hidden_pairs[j].n2 + 1
                    , hidden_pairs[j].i1
                    , hidden_pairs[j].i2
                  );
            /* 適用 */
            for( l = 0; l < MAX_NUM; l++ ){
                if( l == hidden_pairs[j].n1 || l == hidden_pairs[j].n2 ){
                    continue;
                }
                matrix[l][hidden_pairs[j].i1] = NUMPLACE_OFF;
                matrix[l][hidden_pairs[j].i2] = NUMPLACE_OFF;
            }
            printf("-------------適用-------------\n");
            print_place_info_table2_atgroup( ptable, grouptype, id );
            print_matrix( matrix, grouptype, id );
            merge_matrix( matrix, ptable, grouptype, id );
            print_place_info_table2_atgroup( ptable, grouptype, id );
            /* 他の行列への適用 */
            if( grouptype == NUMPLE_GROUP_BLOCK ){
                if( hidden_pairs[j].i1 / 3 == hidden_pairs[j].i2 / 3 ){
                    /* 同一行 */
                    row = (id / 3) * 3 + hidden_pairs[j].i1 / 3;
                    puts("-------------他行への適用(before)-------------");
                    set_matrix( matrix_tmp, ptable, NUMPLE_GROUP_HORIZONTAL_LINE
                            , row );
                    print_matrix( matrix_tmp, NUMPLE_GROUP_HORIZONTAL_LINE, row);
                    print_place_info_table2_atgroup( ptable, NUMPLE_GROUP_HORIZONTAL_LINE, row );
                    i1 = (id % 3) * 3 + hidden_pairs[j].i1 % 3;
                    i2 = (id % 3) * 3 + hidden_pairs[j].i2 % 3;
                    for( l = 0; l < MAX_NUM; l++ ){
                       if( l == i1 || l == i2 ){
                           continue;
                       }
                       matrix_tmp[hidden_pairs[j].n1][l] = NUMPLACE_OFF;
                       matrix_tmp[hidden_pairs[j].n2][l] = NUMPLACE_OFF;
                    }
                    puts("-------------他行への適用( after)-------------");
                    print_matrix( matrix_tmp, NUMPLE_GROUP_HORIZONTAL_LINE, row);
                    merge_matrix( matrix_tmp, ptable, NUMPLE_GROUP_HORIZONTAL_LINE, row);
                    print_place_info_table2_atgroup( ptable, NUMPLE_GROUP_HORIZONTAL_LINE, row );
                }
                if( hidden_pairs[j].i1 % 3 == hidden_pairs[j].i2 % 3 ){
                    /* 同一列 */
                    col = (id % 3) * 3 + hidden_pairs[j].i1 % 3;
                    puts("-------------他列への適用(before)-------------");
                    set_matrix( matrix_tmp, ptable, NUMPLE_GROUP_VERTICAL_LINE, col);
                    print_matrix( matrix_tmp, NUMPLE_GROUP_VERTICAL_LINE, col);
                    print_place_info_table2_atgroup( ptable, NUMPLE_GROUP_VERTICAL_LINE, col );
                    i1 = (id / 3) * 3 + hidden_pairs[j].i1 / 3;
                    i2 = (id / 3) * 3 + hidden_pairs[j].i2 / 3;
                    for( l = 0; l < MAX_NUM; l++){
                        if( l == i1 || l == i2 ){
                            continue;
                        }
                        matrix_tmp[hidden_pairs[j].n1][l] = NUMPLACE_OFF;
                        matrix_tmp[hidden_pairs[j].n2][l] = NUMPLACE_OFF;
                    }
                    puts("-------------他列への適用( after)-------------");
                    print_matrix( matrix_tmp, NUMPLE_GROUP_VERTICAL_LINE, col);
                    merge_matrix( matrix_tmp, ptable, NUMPLE_GROUP_VERTICAL_LINE, col);
                    print_place_info_table2_atgroup( ptable, NUMPLE_GROUP_VERTICAL_LINE, col );
                }
            }
        }
    }
    /* matrixの列ごとの和 */
    for( j = 0; j < MAX_NUM; j++){
        for( k = 0; k < MAX_NUM; k++) {
            if( matrix[k][j] == NUMPLACE_ON ){
                ci_col[j].numbers[ci_col[j].count] = k;
                ci_col[j].count++;
            }
        }
        if( ci_col[j].count == 2){
            for(l = 0; l < MAX_NUM; l++){
                if( nakid_pairs[l].count == 0 ){
                    nakid_pairs[l].i1 = j;
                    nakid_pairs[l].n1 = ci_col[j].numbers[0];
                    nakid_pairs[l].n2 = ci_col[j].numbers[1];
                    nakid_pairs[l].count++;
                    break;
                }else if( nakid_pairs[l].n1 == ci_col[j].numbers[0]
                       && nakid_pairs[l].n2 == ci_col[j].numbers[1]){
                    if( nakid_pairs[l].count == 1 ){
                        nakid_pairs[l].i2 = j;
                    }
                    nakid_pairs[l].count++;
                    break;
                }
            }
        }
            
    }
    /* ペアの抽出 */
    for( j = 0; j < MAX_NUM; j++){
        if( nakid_pairs[j].count == 2 ){
            printf(" nakid_pair group:%d id:%d num(%d,%d) masu(%d,%d)\n"
                    , grouptype
                    , id
                    , nakid_pairs[j].n1 + 1
                    , nakid_pairs[j].n2 + 1
                    , nakid_pairs[j].i1
                    , nakid_pairs[j].i2
                  );
            /* 適用 */
            for( l = 0; l < MAX_NUM; l++ ){
                if( l == nakid_pairs[j].i1 || l == nakid_pairs[j].i2 ){
                    continue;
                }
                matrix[nakid_pairs[j].n1][l] = NUMPLACE_OFF;
                matrix[nakid_pairs[j].n2][l] = NUMPLACE_OFF;
            }
            print_matrix( matrix, grouptype, id );
            print_place_info_table2_atgroup( ptable, grouptype, id );
            merge_matrix( matrix, ptable, grouptype, id);
            print_place_info_table2_atgroup( ptable, grouptype, id );
        }
    }
}
int search_nakid_pair(PLACEINFO_TABLE ptable)
{
    int i;
    for(i = 0; i < MAX_NUM; i++){
        search_nakid_pair_atgroup( ptable, NUMPLE_GROUP_HORIZONTAL_LINE, i);
        search_nakid_pair_atgroup( ptable, NUMPLE_GROUP_VERTICAL_LINE, i);
        search_nakid_pair_atgroup( ptable, NUMPLE_GROUP_BLOCK, i);
    }
    return 1;
}
/*
 * 返り値: 解けた場合1, 解けなかった場合0
 */
int solve_numplace(NUMPLEBOARD board)
{
    int result = 0;
    int i;
    place_info_t table[MAX_NUM][MAX_NUM];
    PLACEINFO_TABLE ptable;
    //result = search_hidden_one(board);

    ptable = table;
    set_place_info_table( ptable, board ); /* board -> ptable */
    print_place_info_table2(ptable);
    
    /*
    result = search_line_block(board);
    set_place_info_table(ptable, board);
    */
    update_place_info( ptable );
    print_place_info_table2(ptable);

    /* ペア探し */
    search_nakid_pair( ptable);

    update_place_info( ptable );
    print_place_info_table2(ptable);

    feedback_to_board( ptable, board ); /* ptable -> board */


    return result;
}
int check_puttable_num(NUMPLEBOARD board, int x, int y)
{
    int x0,x1,y0,y1;
    int i,j;
    /*
     * 00000...00111..11
     * 上のように0に続いて1が並ぶ形を作る。1の個数はMAX_NUM
     */
    int flag = ~((~0) << MAX_NUM );
    for(i = 0; i < MAX_NUM; i++){
        /* 縦横のラインに数字があった場合は
         * 数字のケタにあるbitを0にする*/
        if( board[x][i] > 0 ){
            flag &= ~( 1 << board[x][i] - 1 );
        }
        if( board[i][x] > 0 ){
            flag &= ~( 1 << board[i][y] - 1 );
        }
    }

    x0 = (x / 3) * 3;
    y0 = (y / 3) * 3;
    x1 = x0 + 2;
    y1 = y0 + 2;
    for(i = x0; i <= x1; i++){
        for(j = y0; j <= y1; j++){
            /* ブロックについても同様 */
            if(board[i][j] > 0){
                flag &= ~( 1 << board[i][j] - 1 );
            }
        }
    }
    /* 縦横ブロックに数字があったケタの
     * bitが 0 になっている状態のフラグを
     * 呼び出し元に返却する 
     * 例 3と5だけあった場合
     * 000...000000010100
     */
    return flag;
}
void check_puttable_num2(NUMPLEBOARD board, int x, int y, int *flags)
{
    int i,n;
    int x0,y0;

    x0 = ( x / 3 ) * 3;
    y0 = ( y / 3 ) * 3;
    for(i = 0; i < MAX_NUM; i++){
        //printf("x:%d,y:%d,x0:%d,y0:%d,i:%d, %d,%d,   h:%d,v:%d,b:%d\n",x,y,x0,y0,i,x0+(i/3),y0+(i%3), board[x][i],board[i][y],board[x0+(i/3)][y0+(i%3)]);
        /* Horizontal Lines */
        n = board[x][i];
        if( n > 0){
            flags[n - 1] = NUMPLACE_ON;
        }
        /* Vertical Lines */
        n = board[i][y];
        if( n > 0){
            flags[n - 1] = NUMPLACE_ON;
        }
        /* Blocks */
        n = board[x0 + (i/3)][y0 + (i%3)];
        if( n > 0){
            flags[n - 1] = NUMPLACE_ON;
        }
    }
    for(i = 0; i < MAX_NUM; i++){
        if( flags[i] == NUMPLACE_ON ){
            flags[i] = NUMPLACE_OFF;
        }else if( flags[i] == NUMPLACE_OFF ){
            flags[i] = NUMPLACE_ON;
        }
    }
    //putchar('\n');
}
