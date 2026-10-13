#include "global.h"
extern void *gUnk_020000DC;
void *sub_08219FBC(int, int);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_08031840(void);
void sub_08031964(void);
int sub_0803198C(void *, void *);
void *sub_08031D40(void *config)
{
    void *p = gUnk_020000DC;
    if (!p) {
        p = sub_08219FBC(12, 720);
        if (p) {
            sub_0821A04C(p, sub_08031840, sub_08031964);
            if (sub_0803198C(p, config) < 0) {
                sub_0821A0C0(p);
                return 0;
            }
        }
    }
    return p;
}
