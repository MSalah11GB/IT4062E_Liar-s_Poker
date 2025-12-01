#ifndef POKER_H
#define POKER_H

typedef struct {
    int score; // integer score for comparison (larger = better)
    int rank;  // hand rank category (0..8)
    int tiebreakers[5]; // high cards for tie-breaking
} hand_rank_t;

// cards: array of N strings "As" like "AH" "TD" etc. n <= 7
hand_rank_t poker_best_hand(char cards[][3], int n);

#endif
