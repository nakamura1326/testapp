/* tree.c がスパゲッティコードと化したので新しく作る */
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <limits.h>

/* 指定した回数だけディレクトリツリーを昇ります */
int ascend_dir( char *dir, size_t len, int n );
int tree(const char* root);

int main(void)
{
    char root[] = "/usr/local";
    tree( root );
    return 0;
}
int tree(const char* root){
    typedef struct tag_dirinfo{
        char name[256]; /* ディレクトリ名 */
        int depth;  /* 深さ */
    } dirinfo_t;

    int kaiso[20] = {0};  /* index階層にある残りディレクトリ数 */
    char path[256]; /* ディレクトリパス。opendirで使う */
    dirinfo_t di; /* ディレクトリスタックに積む用のまとめデータ */
    dirinfo_t stack[256]; /* ディレクトリスタック */
    int sp; /* ディレクトリスタックポインタ */
    int i;  /* ループカウンタ用 */
    dirinfo_t sub_stk[256];  /* サブディレクトリのソート用 */
    char op_str[256];   /* 画面出力用文字列 */
    DIR *dp;    /* ディレクトリポインタ */
    struct dirent *ent;    /* ディレクトリエントリポインタ */
    /* ワイド文字-->マルチバイト文字変換用 */
    char lb[MB_LEN_MAX], ls[MB_LEN_MAX], vl[MB_LEN_MAX], hl[MB_LEN_MAX];

    sp = 0;
    strcpy(path, root);
    di.name[0] = '\0';
    di.depth = 0;
    setlocale(LC_CTYPE, "");
    wctomb( lb, L'└' );    /* LEFT_BOTTOM_BAR */
    wctomb( ls, L'├' );    /* LEFT_SIDE_BAR */
    wctomb( hl, L'─' );    /* HORIZONTAL_BAR */
    wctomb( vl, L'│' );    /* VERTICAL_BAR */

    while(1){
        /* 画面出力 */
        op_str[0] = '\0';
        for(i = 0; i < di.depth; i++){
            if( kaiso[i + 1] > 0){
                if( i == di.depth - 1 ){
                    strcat( op_str, ls);
                    strcat( op_str, hl);
                }else{
                    strcat( op_str, vl);
                    strcat( op_str, "  ");
                }
            }else{
                if( i == di.depth - 1 ){
                    strcat( op_str, lb);
                    strcat( op_str, hl);
                }else{
                    strcat( op_str, "    ");
                }
            }
        }
        strcat( op_str, di.name );
        puts( op_str );

        /* サブディレクトリの列挙 */
        strcat( path, "/");
        strcat( path, di.name);
        dp = opendir( path );
        if( dp == NULL ){
            perror( path );
            return 0;
        }
        for(i = 0; ( ent = readdir( dp )) != NULL; ){
            if( ent->d_type != DT_DIR ){
                continue;
            }
            if( ent->d_name[0] == '.' ){
                continue;
            }
        /*
         * ディレクトリエントリからディレクトリ名をコピーしないといけない
         * ディレクトリエントリの文字配列への参照を持つだけだと
         * 上書きされて不正な動作の原因となることがある
        */
            strcpy( sub_stk[i].name, ent->d_name );
            sub_stk[i].depth = di.depth + 1;
            i++;
        }
        closedir( dp );
        /* サブディレクトリのソート */

        /* サブディレクトリをスタックに積み上げる(逆順) */
        while ( i > 0 ){
            stack[sp++] = sub_stk[--i];
            kaiso[di.depth + 1]++;
        }

        /* 次のディレクトリを決定する
         * 未表示ディレクトリが0ではない階層までさかのぼる
         */
        for( i = di.depth + 1; i > 0; i-- ){
            if( kaiso[i] > 0 ){
                di = stack[--sp];
                kaiso[i]--;
                break;
            }
            if( !ascend_dir( path, strlen(path), 1 )){
                puts("error: ascend_dir");
                return 0;
            }
        }
        if( i == 0 ){
            puts("tree end.");
            return 0;
        }
    }
    return 0;
}

int ascend_dir( char *dir, size_t len, int n )
{
    int i = 0;
    char *p;

    p = dir + len - 1;
    while( p >= dir ){
        if( *p-- == '/' ){
            while( p >= dir && *p == '/' ){
                p--;
            }
            if( ++i == n ){
                *++p = '\0';
                return 1;
            }
        }
    }
    return 0;
}


