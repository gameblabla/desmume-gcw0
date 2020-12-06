#include <stdint.h>
#include <stdlib.h>

//from BGR555
#define cR(A) (((A) & 0x001f) << 3)
#define cG(A) (((A) & 0x03e0) >> 2)
#define cB(A) (((A) & 0x7c00) >> 7)
//to BGR555
#define Weight1_1(A, B)  ((((cB(A) + cB(B)) >> 1) & 0xf8) << 7 | (((cG(A) + cG(B)) >> 1) & 0xf8) << 2 | (((cR(A) + cR(B)) >> 1) & 0xf8) >> 3)
#define Weight5_3(A, B)  (((((cB(A) * 5) + (cB(B) * 3)) >> 3) & 0xf8) << 7 | ((((cG(A) * 5) + (cG(B) * 3)) >> 3) & 0xf8) << 2 | ((((cR(A) * 5) + (cR(B) * 3)) >> 3) & 0xf8) >> 3)
#define Weight3_5(A, B)  (((((cB(A) * 3) + (cB(B) * 5)) >> 3) & 0xf8) << 7 | ((((cG(A) * 3) + (cG(B) * 5)) >> 3) & 0xf8) << 2 | ((((cR(A) * 3) + (cR(B) * 5)) >> 3) & 0xf8) >> 3)
#define Weight2_5_1(A, B, C)  ((((cB(C) + (cB(B) * 5) + (cB(A) * 2)) >> 3) & 0xf8) << 7 | (((cG(C) + (cG(B) * 5) + (cG(A) * 2)) >> 3) & 0xf8) << 2 | (((cR(C) + (cR(B) * 5) + (cR(A) * 2)) >> 3) & 0xf8) >> 3)
#define Weight1_5_2(A, B, C)  ((((cB(A) + (cB(B) * 5) + (cB(C) * 2)) >> 3) & 0xf8) << 7 | (((cG(A) + (cG(B) * 5) + (cG(C) * 2)) >> 3) & 0xf8) << 2 | (((cR(A) + (cR(B) * 5) + (cR(C) * 2)) >> 3) & 0xf8) >> 3)
//to RGB565
#define WeightB1_1(A, B)  ((((cR(A) + cR(B)) >> 1) & 0xf8) << 8 | (((cG(A) + cG(B)) >> 1) & 0xfc) << 3 | (((cB(A) + cB(B)) >> 1) & 0xf8) >> 3)
#define WeightB5_3(A, B)  (((((cR(A) * 5) + (cR(B) * 3)) >> 3) & 0xf8) << 8 | ((((cG(A) * 5) + (cG(B) * 3)) >> 3) & 0xfc) << 3 | ((((cB(A) * 5) + (cB(B) * 3)) >> 3) & 0xf8) >> 3)
#define WeightB3_5(A, B)  (((((cR(A) * 3) + (cR(B) * 5)) >> 3) & 0xf8) << 8 | ((((cG(A) * 3) + (cG(B) * 5)) >> 3) & 0xfc) << 3 | ((((cB(A) * 3) + (cB(B) * 5)) >> 3) & 0xf8) >> 3)
#define WeightB2_5_1(A, B, C)  ((((cR(C) + (cR(B) * 5) + (cR(A) * 2)) >> 3) & 0xf8) << 8 | (((cG(C) + (cG(B) * 5) + (cG(A) * 2)) >> 3) & 0xfc) << 3 | (((cB(C) + (cB(B) * 5) + (cB(A) * 2)) >> 3) & 0xf8) >> 3)
#define WeightB1_5_2(A, B, C)  ((((cR(A) + (cR(B) * 5) + (cR(C) * 2)) >> 3) & 0xf8) << 8 | (((cG(A) + (cG(B) * 5) + (cG(C) * 2)) >> 3) & 0xfc) << 3 | (((cB(A) + (cB(B) * 5) + (cB(C) * 2)) >> 3) & 0xf8) >> 3)

void scale_256x384_to_160x240(uint32_t* dst, uint32_t* src)
{
    uint16_t* Src16 = (uint16_t*) src;
    uint16_t* Dst16 = (uint16_t*) dst;

    uint32_t BlockX, BlockY;
    uint16_t* BlockSrc;
    uint16_t* BlockDst;
    for (BlockY = 0; BlockY < 48; BlockY++)
    {
        BlockSrc = Src16 + BlockY * 256 * 8;
        BlockDst = Dst16 + 80 + BlockY * 320 * 5; // 80 is the offset for centering the image into the 320-width destination surface
        for (BlockX = 0; BlockX < 32; BlockX++)
        {
            // 8x8 to 5x5
            // Before: (a)(b)(c)(d)(e)(f)(g)(h)  After: (aaaaabbb)(bbcccccd)(de)(efffffgg)(ggghhhhh)

            // -- Row 1 --;
            uint16_t  a1 = *(BlockSrc               );
            uint16_t  a2 = *(BlockSrc            + 1);
            uint16_t  b1 = *(BlockSrc + 256 *  1    );
            uint16_t  b2 = *(BlockSrc + 256 *  1 + 1);
            *(BlockDst              ) = WeightB5_3(Weight5_3( a1, a2), Weight5_3( b1, b2));
            uint16_t  a3 = *(BlockSrc            + 2);
            uint16_t  a4 = *(BlockSrc            + 3);
            uint16_t  b3 = *(BlockSrc + 256 *  1 + 2);
            uint16_t  b4 = *(BlockSrc + 256 *  1 + 3);
            *(BlockDst           + 1) = WeightB5_3(Weight2_5_1( a2, a3, a4), Weight2_5_1( b2, b3, b4));
            uint16_t  a5 = *(BlockSrc            + 4);
            uint16_t  b5 = *(BlockSrc + 256 *  1 + 4);
            *(BlockDst           + 2) = WeightB5_3(Weight1_1( a4, a5), Weight1_1( b4, b5));
            uint16_t  a6 = *(BlockSrc            + 5);
            uint16_t  a7 = *(BlockSrc            + 6);
            uint16_t  b6 = *(BlockSrc + 256 *  1 + 5);
            uint16_t  b7 = *(BlockSrc + 256 *  1 + 6);
            *(BlockDst           + 3) = WeightB5_3(Weight1_5_2( a5, a6, a7), Weight1_5_2( b5, b6, b7));
            uint16_t  a8 = *(BlockSrc            + 7);
            uint16_t  b8 = *(BlockSrc + 256 *  1 + 7);
            *(BlockDst           + 4) = WeightB5_3(Weight3_5( a7, a8), Weight3_5( b7, b8));

            // -- Row 2 --
            uint16_t  c1 = *(BlockSrc + 256 *  2    );
            uint16_t  c2 = *(BlockSrc + 256 *  2 + 1);
            uint16_t  d1 = *(BlockSrc + 256 *  3    );
            uint16_t  d2 = *(BlockSrc + 256 *  3 + 1);
            *(BlockDst + 320 * 1    ) = WeightB2_5_1(Weight5_3( b1, b2), Weight5_3( c1, c2), Weight5_3( d1, d2));
            uint16_t  c3 = *(BlockSrc + 256 *  2 + 2);
            uint16_t  c4 = *(BlockSrc + 256 *  2 + 3);
            uint16_t  d3 = *(BlockSrc + 256 *  3 + 2);
            uint16_t  d4 = *(BlockSrc + 256 *  3 + 3);
            *(BlockDst + 320 * 1 + 1) = WeightB2_5_1(Weight2_5_1( b2, b3, b4), Weight2_5_1( c2, c3, c4), Weight2_5_1( d2, d3, d4));
            uint16_t  c5 = *(BlockSrc + 256 *  2 + 4);
            uint16_t  d5 = *(BlockSrc + 256 *  3 + 4);
            *(BlockDst + 320 * 1 + 2) = WeightB2_5_1(Weight1_1( b4, b5), Weight1_1( c4, c5), Weight1_1( d4, d5));
            uint16_t  c6 = *(BlockSrc + 256 *  2 + 5);
            uint16_t  c7 = *(BlockSrc + 256 *  2 + 6);
            uint16_t  d6 = *(BlockSrc + 256 *  3 + 5);
            uint16_t  d7 = *(BlockSrc + 256 *  3 + 6);
            *(BlockDst + 320 * 1 + 3) = WeightB2_5_1(Weight1_5_2( b5, b6, b7), Weight1_5_2( c5, c6, c7), Weight1_5_2( d5, d6, d7));
            uint16_t  c8 = *(BlockSrc + 256 *  2 + 7);
            uint16_t  d8 = *(BlockSrc + 256 *  3 + 7);
            *(BlockDst + 320 * 1 + 4) = WeightB2_5_1(Weight3_5( b7, b8), Weight3_5( c7, c8), Weight3_5( d7, d8));

            // -- Row 3 --
            uint16_t  e1 = *(BlockSrc + 256 *  4    );
            uint16_t  e2 = *(BlockSrc + 256 *  4 + 1);
            *(BlockDst + 320 * 2    ) = WeightB1_1(Weight5_3( d1, d2), Weight5_3( e1, e2));
            uint16_t  e3 = *(BlockSrc + 256 *  4 + 2);
            uint16_t  e4 = *(BlockSrc + 256 *  4 + 3);
            *(BlockDst + 320 * 2 + 1) = WeightB1_1(Weight2_5_1( d2, d3, d4), Weight2_5_1( e2, e3, e4));
            uint16_t  e5 = *(BlockSrc + 256 *  4 + 4);
            *(BlockDst + 320 * 2 + 2) = WeightB1_1(Weight1_1( d4, d5), Weight1_1( e4, e5));
            uint16_t  e6 = *(BlockSrc + 256 *  4 + 5);
            uint16_t  e7 = *(BlockSrc + 256 *  4 + 6);
            *(BlockDst + 320 * 2 + 3) = WeightB1_1(Weight1_5_2( d5, d6, d7), Weight1_5_2( e5, e6, e7));
            uint16_t  e8 = *(BlockSrc + 256 *  4 + 7);
            *(BlockDst + 320 * 2 + 4) = WeightB1_1(Weight3_5( d7, d8), Weight3_5( e7, e8));

            // -- Row 4 --
            uint16_t  f1 = *(BlockSrc + 256 *  5    );
            uint16_t  f2 = *(BlockSrc + 256 *  5 + 1);
            uint16_t  g1 = *(BlockSrc + 256 *  6    );
            uint16_t  g2 = *(BlockSrc + 256 *  6 + 1);
            *(BlockDst + 320 * 3    ) = WeightB1_5_2(Weight5_3( e1, e2), Weight5_3( f1, f2), Weight5_3( g1, g2));
            uint16_t  f3 = *(BlockSrc + 256 *  5 + 2);
            uint16_t  f4 = *(BlockSrc + 256 *  5 + 3);
            uint16_t  g3 = *(BlockSrc + 256 *  6 + 2);
            uint16_t  g4 = *(BlockSrc + 256 *  6 + 3);
            *(BlockDst + 320 * 3 + 1) = WeightB1_5_2(Weight2_5_1( e2, e3, e4), Weight2_5_1( f2, f3, f4), Weight2_5_1( g2, g3, g4));
            uint16_t  f5 = *(BlockSrc + 256 *  5 + 4);
            uint16_t  g5 = *(BlockSrc + 256 *  6 + 4);
            *(BlockDst + 320 * 3 + 2) = WeightB1_5_2(Weight1_1( e4, e5), Weight1_1( f4, f5), Weight1_1( g4, g5));
            uint16_t  f6 = *(BlockSrc + 256 *  5 + 5);
            uint16_t  f7 = *(BlockSrc + 256 *  5 + 6);
            uint16_t  g6 = *(BlockSrc + 256 *  6 + 5);
            uint16_t  g7 = *(BlockSrc + 256 *  6 + 6);
            *(BlockDst + 320 * 3 + 3) = WeightB1_5_2(Weight1_5_2( e5, e6, e7), Weight1_5_2( f5, f6, f7), Weight1_5_2( g5, g6, g7));
            uint16_t  f8 = *(BlockSrc + 256 *  5 + 7);
            uint16_t  g8 = *(BlockSrc + 256 *  6 + 7);
            *(BlockDst + 320 * 3 + 4) = WeightB1_5_2(Weight3_5( e7, e8), Weight3_5( f7, f8), Weight3_5( g7, g8));

            // -- Row 5 --
            uint16_t  h1 = *(BlockSrc + 256 *  7    );
            uint16_t  h2 = *(BlockSrc + 256 *  7 + 1);
            *(BlockDst + 320 * 4    ) = WeightB3_5(Weight5_3( g1, g2), Weight5_3( h1, h2));
            uint16_t  h3 = *(BlockSrc + 256 *  7 + 2);
            uint16_t  h4 = *(BlockSrc + 256 *  7 + 3);
            *(BlockDst + 320 * 4 + 1) = WeightB3_5(Weight2_5_1( g2, g3, g4), Weight2_5_1( h2, h3, h4));
            uint16_t  h5 = *(BlockSrc + 256 *  7 + 4);
            *(BlockDst + 320 * 4 + 2) = WeightB3_5(Weight1_1( g4, g5), Weight1_1( h4, h5));
            uint16_t  h6 = *(BlockSrc + 256 *  7 + 5);
            uint16_t  h7 = *(BlockSrc + 256 *  7 + 6);
            *(BlockDst + 320 * 4 + 3) = WeightB3_5(Weight1_5_2( g5, g6, g7), Weight1_5_2( h5, h6, h7));
            uint16_t  h8 = *(BlockSrc + 256 *  7 + 7);
            *(BlockDst + 320 * 4 + 4) = WeightB3_5(Weight3_5( g7, g8), Weight3_5( h7, h8));

            BlockSrc += 8;
            BlockDst += 5;
        }
    }
}
