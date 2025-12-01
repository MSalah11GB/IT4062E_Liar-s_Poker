// Brute-force 7->5 evaluator: constructs all 5-card combinations and ranks them.
// Ranking: HIGH_CARD=0, PAIR=1, TWO_PAIR=2, THREE=3, STRAIGHT=4, FLUSH=5, FULL_HOUSE=6, FOUR=7, STRAIGHT_FLUSH=8
#include "poker.h"
#include <string.h>
#include <stdlib.h>

static int rank_val(char r){
    const char *R = "23456789TJQKA";
    const char *p = strchr(R, r);
    if (!p) return 0;
    return (int)(p - R) + 2;
}

static int is_flush(char cards[][3], int idx[]){
    char s = cards[idx[0]][1];
    for(int i=1;i<5;i++) if (cards[idx[i]][1] != s) return 0;
    return 1;
}

static int is_straight_vals(int vals[5], int out_high){
    // vals expected descending
    // check normal straight
    int consec = 1;
    for(int i=1;i<5;i++){
        if (vals[i] == vals[i-1]-1) consec++;
    }
    if (consec == 5) return 1;
    // check wheel A-2-3-4-5
    if (vals[0] == 14 && vals[1]==5 && vals[2]==4 && vals[3]==3 && vals[4]==2) return 1;
    return 0;
}

static void sort_desc(int a[], int n){
    for(int i=0;i<n;i++) for(int j=i+1;j<n;j++) if(a[j]>a[i]){ int t=a[i]; a[i]=a[j]; a[j]=t; }
}

static hand_rank_t rank_five(char cards[][3], int idx[]){
    int vals[5];
    for(int i=0;i<5;i++) vals[i] = rank_val(cards[idx[i]][0]);
    // sort desc
    sort_desc(vals,5);
    int counts[15] = {0};
    for(int i=0;i<5;i++) counts[vals[i]]++;
    int four=0, three=0, pairs=0;
    for(int v=2; v<=14; v++){
        if (counts[v]==4) four = v;
        if (counts[v]==3) three = v;
        if (counts[v]==2) pairs++;
    }
    hand_rank_t hr;
    memset(&hr,0,sizeof(hr));
    int flush = is_flush(cards, idx);
    int straight = is_straight_vals(vals, 0);
    if (straight && flush) {
        hr.rank = 8;
        hr.score = 8000000 + vals[0];
        return hr;
    }
    if (four) {
        hr.rank = 7;
        hr.score = 7000000 + four*100;
        return hr;
    }
    if (three && pairs==1) {
        hr.rank = 6;
        hr.score = 6000000 + three*100;
        return hr;
    }
    if (flush) {
        hr.rank = 5;
        hr.score = 5000000 + vals[0]*10000 + vals[1]*100 + vals[2];
        return hr;
    }
    if (straight) {
        hr.rank = 4;
        hr.score = 4000000 + vals[0];
        return hr;
    }
    if (three) {
        hr.rank = 3;
        hr.score = 3000000 + three*100;
        return hr;
    }
    if (pairs==2) {
        // two pair: find top two pairs
        int high=0, low=0;
        for(int v=14; v>=2; v--){
            if (counts[v]==2) {
                if (!high) high=v;
                else if (!low) low=v;
            }
        }
        hr.rank = 2;
        hr.score = 2000000 + high*100 + low;
        return hr;
    }
    if (pairs==1) {
        int p=0;
        for(int v=14; v>=2; v--) if (counts[v]==2) { p=v; break; }
        hr.rank = 1;
        hr.score = 1000000 + p*100;
        return hr;
    }
    // high card
    hr.rank = 0;
    hr.score = vals[0]*10000 + vals[1]*100 + vals[2];
    return hr;
}

hand_rank_t poker_best_hand(char cards[][3], int n) {
    hand_rank_t best; best.score = -1;
    if (n < 5) {
        // pad? treat as high card
        hand_rank_t h; memset(&h,0,sizeof(h)); h.score = 0; return h;
    }
    // iterate combinations C(n,5) (n <= 7)
    int idx[5];
    for (idx[0]=0; idx[0]<n-4; idx[0]++)
    for (idx[1]=idx[0]+1; idx[1]<n-3; idx[1]++)
    for (idx[2]=idx[1]+1; idx[2]<n-2; idx[2]++)
    for (idx[3]=idx[2]+1; idx[3]<n-1; idx[3]++)
    for (idx[4]=idx[3]+1; idx[4]<n; idx[4]++) {
        hand_rank_t r = rank_five(cards, idx);
        if (r.score > best.score) best = r;
    }
    return best;
}
