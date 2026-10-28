#include "global.h"
#include "input.h"
typedef struct {
    s16 prompt;
    u8 wait, hide;
    int (*run)(void *);
} Step;
extern Step gUnk_0824DBC0[];
int ResultTutorial_ShowPrompt(void *);
int ResultTutorial_HidePrompt(void *);
int sub_08249240(void *, void *, void *);
void Sound_PlaySong(int);
void Sound_StopSong(int);
void sub_0801C24C(void *);
void ResultTutorial_UpdateSequence(u8 *p)
{
    int index = p[0xb4] >> 2;
    switch (p[0xb4] & 3) {
        case 0:
            if (gUnk_0824DBC0[index].prompt >= 0) {
                p[0xb6] = gUnk_0824DBC0[index].prompt;
                if (ResultTutorial_ShowPrompt(p)) {
                    ++p[0xb4];
                    p[0xaf] = 0;
                }
                break;
            }
            ++p[0xb4];
            p[0xaf] = 0;
        case 1:
            if (gUnk_0824DBC0[index].wait) {
                if (gPadState[1] & 1) {
                    ++p[0xb4];
                    Sound_PlaySong(0x107);
                }
                break;
            }
            ++p[0xb4];
            p[0xaf] = 0;
        case 2:
            if (gUnk_0824DBC0[index].hide) {
                if (ResultTutorial_HidePrompt(p)) {
                    int next = p[0xb4] + 1;
                    int zero;
                    zero = 0;
                    p[0xb4] = next;
                    p[0xaf] = zero;
                }
                break;
            }
            ++p[0xb4];
            p[0xaf] = 0;
        case 3:
            if (gUnk_0824DBC0[index].run) {
                if (sub_08249240(p, &gUnk_0824DBC0[index].run, gUnk_0824DBC0[index].run)) {
                    int next = p[0xb4] + 1;
                    int zero;
                    zero = 0;
                    p[0xb4] = next;
                    p[0xaf] = zero;
                    Sound_StopSong(0x3ed);
                    Sound_StopSong(0x3ee);
                }
                --*(u16 *)(p + 0xac);
            } else {
                ++p[0xb4];
                p[0xaf] = 0;
            }
            break;
    }
    sub_0801C24C(p);
}
