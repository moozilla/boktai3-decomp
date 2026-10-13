#include "global.h"
extern u8 gUnk_0824DB8A[];
void ResultTutorial_MovePlayer(void *, int);
void ResultGame_ScoreAlignment(void *);
#define I(o) (*(int *)(p + (o)))
#define H(o) (*(s16 *)(p + (o)))
int ResultTutorial_DemonstrateTracking(u8 *p)
{
    int speed = gUnk_0824DB8A[p[0xae]];
    switch (p[0xaf]) {
        case 0: {
            int next = p[0xaf] + 1;
            int zero;
            zero = 0;
            p[0xaf] = next;
            H(0xa8) = zero;
            H(0xaa) = zero;
            break;
        }
        case 1:
        case 5:
        case 9:
            I(0x9c) += speed;
            if (I(0x9c) > 4096) {
                int next = p[0xaf] + 1;
                int zero;
                zero = 0;
                p[0xaf] = next;
                H(0xaa) = zero;
            }
            break;
        case 2:
        case 6:
        case 10:
            if (++H(0xaa) > 30)
                ++p[0xaf];
            break;
        case 3:
        case 7:
        case 11:
            I(0x9c) -= speed;
            if (I(0x9c) <= 3071) {
                int next = p[0xaf] + 1;
                int zero;
                zero = 0;
                p[0xaf] = next;
                H(0xaa) = zero;
            }
            break;
        case 4:
        case 8:
        case 12:
            if (++H(0xaa) > 10) {
                if (++p[0xaf] > 11)
                    return 1;
            }
            break;
    }
    if (I(0x8c) < I(0x9c) - 96)
        ResultTutorial_MovePlayer(p, 1);
    else
        ResultTutorial_MovePlayer(p, 0);
    if ((H(0xa8) & 15) == 0)
        ResultGame_ScoreAlignment(p);
    ++H(0xa8);
    return 0;
}
