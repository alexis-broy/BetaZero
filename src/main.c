#include <stdio.h>

#define u64 unsigned long long

#define WHITE 1
#define BLACK 2
#define EMPTY 0

enum
{
        WR = 1, WN, WB, WQ, WK, WP,
        BR, BN, BB, BQ, BK, BP
};

u64 white_rooks_bb, white_knights_bb, white_bishops_bb, white_queens_bb, white_king_bb, white_pawns_bb;
u64 black_rooks_bb, black_knights_bb, black_bishops_bb, black_queens_bb, black_king_bb, black_pawns_bb;

u64 main_bb = 0ULL;

#define white_occ() (white_rooks_bb | white_knights_bb | white_bishops_bb | white_king_bb | white_pawns_bb)
#define black_occ() (black_rooks_bb | black_knights_bb | black_bishops_bb | black_king_bb | black_pawns_bb)

static inline void update_main_bb()
{
        main_bb = white_occ() | black_occ();
}

static inline void set_bit(u64 *bb, int sq)
{
        *bb |= (1ULL << sq);
}

static inline int get_bit(u64 bb, int sq)
{
        return bb & (1ULL << sq);
}

static inline void pop_bit(u64 *bb, int sq)
{
        *bb &= ~(1ULL << sq);
}

static inline int get_sq(int rank, int file)
{
        return rank * 8 + file;
}

static u64 *get_piece_bb(int piece)
{
        switch (piece)
        {
                case WR: return &white_rooks_bb;
                case WN: return &white_knights_bb;
                case WB: return &white_bishops_bb;
                case WQ: return &white_queens_bb;
                case WK: return &white_king_bb;
                case WP: return &white_pawns_bb;

                case BR: return &black_rooks_bb;
                case BN: return &black_knights_bb;
                case BB: return &black_bishops_bb;
                case BQ: return &black_queens_bb;
                case BK: return &black_king_bb;
                case BP: return &black_pawns_bb;
                
                default: return NULL;
        }
}

static int get_piece_at(int sq)
{
        u64 mask = 1ULL << sq;

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


static int get_piece_color_at(int sq)
{
        u64 mask = 1ULL << sq;

        if (white_rooks_bb & mask) return WHITE;
        else if (white_knights_bb & mask) return WHITE;
        else if (white_bishops_bb & mask) return WHITE;
        else if (white_queens_bb & mask) return WHITE;
        else if (white_king_bb & mask) return WHITE;
        else if (white_pawns_bb & mask) return WHITE;

        else if (black_rooks_bb & mask) return BLACK;
        else if (black_knights_bb & mask) return BLACK;
        else if (black_bishops_bb & mask) return BLACK;
        else if (black_queens_bb & mask) return BLACK;
        else if (black_king_bb & mask) return BLACK;
        else if (black_pawns_bb & mask) return BLACK;

        return EMPTY; 
}

void move_piece(int from_sq, int to_sq)
{
        int piece = get_piece_at(from_sq);
        if (piece == EMPTY) return;
        
        int piece_color = get_piece_color_at(from_sq);
        int target_color = get_piece_color_at(to_sq);

        if (piece_color == target_color)
        {
                printf("cannot move to a square with its own piece color\n");
                return;
        }

        u64 *from_bb = get_piece_bb(piece);
        if (!from_bb) return;
        pop_bit(from_bb, from_sq);

        if (target_color != EMPTY)
        {
                int target_piece = get_piece_at(to_sq);
                u64 *to_bb = get_piece_bb(target_piece);
                if (to_bb) pop_bit(to_bb, to_sq);
        }

        set_bit(from_bb, to_sq);

        update_main_bb();
}

static const char *symbols[] =
{
        [EMPTY] = ".",
        [WR] = "♖", [WN] = "♘", [WB] = "♗", [WQ] = "♕", [WK] = "♔", [WP] = "♙",
        [BR] = "♜", [BN] = "♞", [BB] = "♝", [BQ] = "♛", [BK] = "♚", [BP] = "♟"
};

static void print_bb(u64 bb, int show_notation)
{
        for (int rank = 7; rank >= 0; rank--)
        {
                if (show_notation) printf("%d ", rank + 1);
                
                for (int file = 0; file < 8; file++)
                {
                        int sq = get_sq(rank, file);
                        int piece = get_piece_at(sq);
                        printf("%s ", symbols[piece]);
                }
                printf("\n");
        }

        if (show_notation) printf("  a b c d e f g h\n\n");
}

static inline void clear_bbs()
{
        white_rooks_bb = 0ULL;
        white_knights_bb = 0ULL;
        white_bishops_bb = 0ULL;
        white_queens_bb = 0ULL;
        white_king_bb = 0ULL;
        white_pawns_bb = 0ULL;

        black_rooks_bb = 0ULL;
        black_knights_bb = 0ULL;
        black_bishops_bb = 0ULL;
        black_queens_bb = 0ULL;
        black_king_bb = 0ULL;
        black_pawns_bb = 0ULL;
}

static inline void init_bbs()
{
        clear_bbs();
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
        for (int i = 48; i <= 55; i++) set_bit(&black_pawns_bb, i);

        update_main_bb();

}

int main()
{
        init_bbs();

        print_bb(main_bb, 1);

        move_piece(6, 21);

        print_bb(main_bb, 1);

        return 0;
}
