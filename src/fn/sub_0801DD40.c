#include "global.h"
#define ResultTutorial_UpdateSequence sub_0801DD40
typedef struct {
    s16 prompt;
    u8 wait, hide;
    int (*run)(void *);
} Step;
extern Step gUnk_0824DBC0[];
extern u16 gUnk_03005260[];
int sub_0801D174(void *);
int sub_0801D1C8(void *);
int sub_08249240(void *, void *, void *);
void sub_0822B2F8(int);
void sub_0822B358(int);
void sub_0801C24C(void *);
void ResultTutorial_UpdateSequence(u8 *p)
{
    int index = p[0xb4] >> 2;
    switch (p[0xb4] & 3) {
        case 0:
            if (gUnk_0824DBC0[index].prompt >= 0) {
                p[0xb6] = gUnk_0824DBC0[index].prompt;
                if (sub_0801D174(p)) {
                    ++p[0xb4];
                    p[0xaf] = 0;
                }
                break;
            }
            ++p[0xb4];
            p[0xaf] = 0;
        case 1:
            if (gUnk_0824DBC0[index].wait) {
                if (gUnk_03005260[1] & 1) {
                    ++p[0xb4];
                    sub_0822B2F8(0x107);
                }
                break;
            }
            ++p[0xb4];
            p[0xaf] = 0;
        case 2:
            if (gUnk_0824DBC0[index].hide) {
                if (sub_0801D1C8(p)) {
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
                    sub_0822B358(0x3ed);
                    sub_0822B358(0x3ee);
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
