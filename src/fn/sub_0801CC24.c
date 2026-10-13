// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern int gUnk_0824DB94[];
extern void *gUnknown_02000580;
void sub_0801C17C(void *);
void sub_0801C1C4(void *);
void sub_0815BE5C(void *, int);
void ResultMenu_SendCommand(void *, int, int, void *);
void Sound_PlaySong(int);
void ResultGame_HandlePress(void *);
void ResultGame_ScorePress(void *);
void sub_0801C16C(void *);
void sub_0801C190(void *);
void sub_0801C69C(void *, void *);
void ResultGame_TrackTarget(void *);
#define I(o) (*(int *)(p + (o)))
#define H(o) (*(s16 *)(p + (o)))
void ResultGame_TimedPress(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            I(0x8c) = 0;
            p[0xb3] = 0;
            I(0x9c) = 1536;
            sub_0801C17C(p);
            sub_0801C1C4(p);
            {
                u8 *state = p + 0xb0;
                int minus;
                *state = 255;
                p[0xb1] = 0;
                ++p[0xaf];
                {
                    u8 *out = p + 0xbf;
                    minus = -1;
                    *out = minus;
                }
            }
            sub_0815BE5C(gUnknown_02000580, 1);
            ResultMenu_SendCommand(p, 0x35, 0, 0);
            Sound_PlaySong(0x4b2);
            break;
        case 1: {
            int zero;
            int zeroByte;
            int next = p[0xaf] + 1;
            s16 *timer;
            zero = 0;
            asm("" : "+r"(zero));
            p[0xaf] = next;
            timer = (s16 *)(p + 0xaa);
            zeroByte = 0;
            *timer = 60;
            H(0xa8) = zero;
            p[0xb0] = 255;
            p[0xb1] = zeroByte;
            break;
        }
        case 2:
            if (H(0xaa) <= 0)
                ++p[0xaf];
            break;
        case 3:
            I(0x8c) += 1024;
            p[0xb0] = 255;
            if (I(0x8c) > 5631) {
                I(0x8c) = 5632;
                {
                    u8 *state = p + 0xaf;
                    int next;
                    int zero;
                    next = *state + 1;
                    zero = 0;
                    *state = next;
                    p[0xb0] = zero;
                }
            }
            break;
        case 4:
            I(0x8c) -= gUnk_0824DB94[I(0x50)];
            ResultGame_HandlePress(p);
            if (I(0x8c) <= 0) {
                {
                    int signedState = (s8)p[0xb0];
                    if (signedState == 0) {
                        ResultGame_ScorePress(p);
                        sub_0801C16C(p);
                        ResultMenu_SendCommand(p, 0x34, 0, 0);
                        p[0xb1] = 1;
                    }
                }
                p[0xb0] = 255;
                I(0x8c) = 0;
                p[0xaf] = 3;
                if (p[0xc4] == 1 || H(0xa8) > 899) {
                    sub_0801C69C(p, ResultGame_TrackTarget);
                    sub_0801C190(p);
                    sub_0801C16C(p);
                    if (p[0xc4] == 1)
                        p[0xc4] = 2;
                    else {
                        ResultMenu_SendCommand(p, 0x36, 0, 0);
                        sub_0815BE5C(gUnknown_02000580, 0);
                    }
                }
            }
            break;
    }
    p[0xb3] = Div(H(0xa8) << 5, 900);
    ++H(0xa8);
    --H(0xaa);
    if (H(0xa8) > 900)
        H(0xa8) = 900;
}
