#include <stdio.h>
#include <stdint.h>


enum
{
        EMPTY = 0,
        WR, WN, WB, WQ, WK, WP,
        BR, BN, BB, BQ, BK, BP
};

uint64_t white_rooks_bb = 0ULL;
uint64_t white_knights_bb = 0ULL;
uint64_t white_bishops_bb = 0ULL;
uint64_t white_queens_bb = 0ULL;
uint64_t white_king_bb = 0ULL;
uint64_t white_pawns_bb = 0ULL;

uint64_t black_rooks_bb = 0ULL;
uint64_t black_knights_bb = 0ULL;
uint64_t black_bishops_bb = 0ULL;
uint64_t black_queens_bb = 0ULL;
uint64_t black_king_bb = 0ULL;
uint64_t black_pawns_bb = 0ULL;

uint64_t main_bb = 0ULL;

static const char *symbols[] =
{
        [EMPTY] = ".",
        [WR] = "♖", [WN] = "♘", [WB] = "♗", [WQ] = "♕", [WK] = "♔", [WP] = "♙",
        [BR] = "♜", [BN] = "♞", [BB] = "♝", [BQ] = "♛", [BK] = "♚", [BP] = "♟"
};

#define white_occ() (white_rooks_bb | white_knights_bb | white_bishops_bb | white_queens_bb | white_king_bb | white_pawns_bb)
#define black_occ() (black_rooks_bb | black_knights_bb | black_bishops_bb | black_queens_bb | black_king_bb | black_pawns_bb)

static inline void update_main_bb(void)
{
        main_bb = white_occ() | black_occ();
}

static inline void set_bit(uint64_t *bb, int bit)
{
        *bb |= (1ULL << bit);
}

static inline void clear_bit(uint64_t *bb, int bit)
{
        *bb &= ~(1ULL << bit);
}

static inline int get_sq(int rank, int file)
{
        return rank * 8 + file;
}

static int get_piece_at(uint64_t bb, int sq)
{
        uint64_t mask = (1ULL << sq);
        
        if (white_rooks_bb & mask) return WR;
        else if (white_knights_bb & mask) return WN;
        else if (white_bishops_bb & mask) return WB;
        else if (white_queens_bb & mask) return WQ;
        else if (white_king_bb & mask) return WK;
        else if (white_pawns_bb & mask) return WP;

        else if (black_rooks_bb & mask) return BR;
        else if (black_knights_bb & mask) return BN;
        else if (black_bishops_bb & mask) return BB;
        else if (black_queens_bb & mask) return BQ;
        else if (black_king_bb & mask) return BK;
        else if (black_pawns_bb & mask) return BP;

        return EMPTY;
}

int is_legal_knight_move(uint64_t bb, int from_sq, int to_sq)
{
        return 0;
}

void move_piece(uint64_t bb, int from_sq, int to_sq)
{
        int piece = get_piece_at(bb, from_sq);
        if (piece == EMPTY) return;

        if (piece == WN || piece == BN)
        {
                int is_legal_n_move = is_legal_knight_move(bb, from_sq, to_sq);
        }
}

static inline void print_bb(uint64_t bb, int show_notation)
{
        for (int rank = 7; rank >= 0; rank--)
        {
                if (show_notation) printf("%d ", rank + 1);
                for (int file = 0; file < 8; file++)
                {
                        int sq = get_sq(rank, file);
                        int piece = get_piece_at(main_bb, sq);
                        printf("%s ", symbols[piece]);
                }
                printf("\n");
        }
        if (show_notation) printf("  a b c d e f g h\n\n");
}

static inline void init_bbs(void)
{
        set_bit(&white_rooks_bb, 0); set_bit(&white_rooks_bb, 7);
        set_bit(&white_knights_bb, 1); set_bit(&white_knights_bb, 6);
        set_bit(&white_bishops_bb, 2); set_bit(&white_bishops_bb, 5);
        set_bit(&white_queens_bb, 3);
        set_bit(&white_king_bb, 4);
        for (int i = 8; i <= 15; i++) set_bit(&white_pawns_bb, i);

        set_bit(&black_rooks_bb, 56); set_bit(&black_rooks_bb, 63);
        set_bit(&black_knights_bb, 57); set_bit(&black_knights_bb, 62);
        set_bit(&black_bishops_bb, 58); set_bit(&black_bishops_bb, 61);
        set_bit(&black_queens_bb, 59);
        set_bit(&black_king_bb, 60);
        for (int i = 48; i <= 56; i++) set_bit(&black_pawns_bb, i);

        update_main_bb();
}

int main(void)
{
        init_bbs();
        print_bb(main_bb, 0);
        move_piece(white_knights_bb, 6, 21);
        return 0;
}
