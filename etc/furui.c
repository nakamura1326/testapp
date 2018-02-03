/* Sieve of Eratosthenes
 * エラトステネスのふるい
 *
 * to compile
 * $ gcc -O2 -lrt -lm furui.c
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

void print_elapsedtime(struct timespec *s, struct timespec *e)
{
    if( s->tv_nsec > e->tv_nsec ){
        printf("%10ld.%09ld(sec)\n", e->tv_sec - s->tv_sec - 1
                , e->tv_nsec + 1000000000 - s->tv_nsec);
    }else{
        printf("%10ld.%09ld(sec)\n", e->tv_sec - s->tv_sec
                , e->tv_nsec - s->tv_nsec);
    }
}

void print_sieve(char *sieve, size_t len)
{
    int i, c;
    for(i = 0, c = 0; i < len; i++){
        if( sieve[i] == 0){
            //printf("%d ", ((i + 1) << 1 ) + 1);
            c++;
        }
    }
    printf("%d prime numbers finded.", c);
    putchar('\n');
}
/*
 * a number <--> 1byte
 */
void do_eratosthenes_sieve(int max)
{
    char* sieve;
    int limit;
    int len;
    char *p, *p0, *end;
    int r;
    struct timespec s,e;

    clock_gettime(CLOCK_REALTIME, &s);

    limit = (int)(sqrt(max) + 1.0);
    len = (max + 1) / 2 - 1;
    if((sieve = calloc( len, 1 )) == NULL){
        fprintf(stderr, "calloc error occured.\n");
        return;
    }
    clock_gettime(CLOCK_REALTIME, &e);
    printf("%6s", "calloc");
    print_elapsedtime( &s, &e );
    
    clock_gettime(CLOCK_REALTIME, &s);

    end = sieve + len - 1;
    p0 = sieve;
    r = ((p0 - sieve + 1) << 1 ) + 1;
    p = p0 + r;
    while( r < limit ){
        while( p <= end ){
            *p = 1;
            p += r;
        }
        while( *++p0 ){
            ;
        }
        r = ((p0 - sieve + 1) * 2 ) + 1;
        p = p0 + r;
    }

    clock_gettime(CLOCK_REALTIME, &e);
    printf("%6s", "sieve");
    print_elapsedtime( &s, &e );

    print_sieve( sieve, len );
    free(sieve);
}
void print_sieve2(char *sieve, size_t len, int max)
{
    int i, j, c, n;
    for(i = 0, c = 0; i < len; i++){
        for(j = 0; j < 8; j++){
            if(( sieve[i] & 1 << j ) == 0 ){
                if(( n = i * 16 + j * 2 + 3 ) <= max ){
                    //printf("%d ", n);
                    c++;
                }
            }
        }
    }
    putchar('\n');
    printf("%d prime numbers finded.", c);
    putchar('\n');
}
/*
 * memory saving version
 * a number <--> 1bit
 */
void do_eratosthenes_sieve2(int max)
{
    char *sieve;
    int limit;
    int len;
    char *p, *p0, *end;
    int pn;                 /* prime number */
    int ofs_p0, ofs, q, r;
    struct timespec s,e;
    
    clock_gettime(CLOCK_REALTIME, &s);
    limit = (int)(sqrt(max) + 1.0);
    len = ( max + 13 ) / 16;
    if((sieve = calloc( len, 1 )) == NULL){
        fprintf(stderr, "calloc error occured.\n");
        return;
    }
    clock_gettime(CLOCK_REALTIME, &e);
    printf("%6s", "calloc");
    print_elapsedtime( &s, &e );
    
    clock_gettime(CLOCK_REALTIME, &s);
    
    end = sieve + len;
    p0 = sieve;
    ofs_p0 = 0;
    pn = (p0 - sieve) * 16 + 3;
    /* 
     * " n >> 3 " equals " n / 8 "
     * " n & 7 " equals " n % 8 "
     */
    q = pn >> 3;
    r = pn & 7;
    p = p0;
    ofs = ofs_p0 + 3;
    while( pn < limit ){
        /* do sieve */
        while(p < end){
            *p |= 1 << ofs;
            p += q + ( ofs + r >> 3 );
            ofs = ( ofs + r ) & 7;
        }
        /* search next prime number */
        while(1){
            if( ++ofs_p0 == 8 ){
                p0++;
                ofs_p0 = 0;
            }
            if(( *p0 & 1 << ofs_p0 ) == 0 ){
                break;
            }
        }
        pn = ( p0 - sieve ) * 16 + ofs_p0 * 2 + 3;
        q = pn >> 3;
        r = pn & 7;
        p = p0 + q + ( ofs_p0 + r >> 3 );
        ofs = ofs_p0 + r & 7;
    }

    clock_gettime(CLOCK_REALTIME, &e);
    printf("%6s", "sieve");
    print_elapsedtime( &s, &e );

    print_sieve2( sieve, len, max );
    free(sieve);

}
int main(int argc, char** argv){
    int i;
    if( argc <= 1 ){
        fprintf(stderr, "furui [num (>0)]\n");
        return 0;
    }
    if(( i = atoi(argv[1])) <= 0 ){
        fprintf(stderr, "furui [num (>0)]\n");
        return 0;
    }

    do_eratosthenes_sieve2( i );
    return 0;
}
