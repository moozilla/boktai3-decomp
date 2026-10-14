#include "global.h"
typedef struct {
    u32 words[8];
} Resource;
Resource *ScriptProc_Start(int, int);
void ResourceHeader_Relocate(void *, void *);
int ResultTutorial_ReadScript(void *);
void ResultMenu_CreateGraphics(void *);
void sub_0801B534(void *, void *);
void ResultTutorial_Open(void *);
extern u32 gUnk_0300523C;
void sub_0822AE7C(int);
int ResultTutorial_Initialize(u8 *p, void *owner)
{
    Resource *r;
    int four;
    u8 *unused; /* unused: changes agbcc's register allocation */
    int zero;
    *((void **)(p + 0x78)) = owner;
    r = ScriptProc_Start(0xcb05, 0x530d);
    if (!r) {
        return -1;
    }
    *((Resource *)(p + 0x104)) = *r;
    ResourceHeader_Relocate(p + 0x104, r);
    r = ScriptProc_Start(0xcb05, 0x5d04);
    if (!r) {
        return -1;
    }
    *((Resource *)(p + 0x124)) = *r;
    ResourceHeader_Relocate(p + 0x124, r);
    if (ResultTutorial_ReadScript(p) < 0) {
        return -1;
    }
    unused = p + 0xae;
    {
        u8 *out = unused;
    }
    zero = 0;
    four = 4;
    p[0xae] = four;
    p[0xb4] = zero;
    p[0xb6] = zero;
    *((int *)(p + 0x50)) = 3;
    ResultMenu_CreateGraphics(p);
    sub_0801B534(p, ResultTutorial_Open);
    sub_0822AE7C(0xaa);
    gUnk_0300523C |= four;
    return 0;
}
