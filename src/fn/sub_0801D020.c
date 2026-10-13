#include "global.h"
typedef struct {
    u32 words[8];
} Resource;
Resource *sub_0821A520(int, int);
void sub_082196C4(void *, void *);
int ResultMenu_ReadScript(void *);
void ResultMenu_CreateGraphics(void *);
void sub_0801B534(void *, void *);
void sub_0801C5E4(void *);
int ResultMenu_Initialize(u8 *p, void *owner)
{
    Resource *r;
    *(void **)(p + 0x78) = owner;
    r = sub_0821A520(0xcb05, 0x530d);
    if (!r)
        return -1;
    *(Resource *)(p + 0x104) = *r;
    sub_082196C4(p + 0x104, r);
    r = sub_0821A520(0xcb05, 0x5d04);
    if (!r)
        return -1;
    *(Resource *)(p + 0x124) = *r;
    sub_082196C4(p + 0x124, r);
    if (ResultMenu_ReadScript(p) < 0)
        return -1;
    ResultMenu_CreateGraphics(p);
    sub_0801B534(p, sub_0801C5E4);
    return 0;
}
