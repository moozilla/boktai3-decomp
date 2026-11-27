#include "global.h"
struct P {
    u8 pad[4];
    u8 b4;
    u8 b5;
    u8 b6;
    u8 b7;
    u8 pad1[6];
    u16 h0e;
    u16 h10;
    u8 pad2[2];
    u32 w14;
    u8 pad3[0x44];
    void *w5c;
    u8 q[0x1c];
    u32 w7c;
};
struct P *TextWindow_Find(u32);
void TextWindow_BlankRect(u32, u32, u32, u32);
void TextState_SetBounds(u8 *, u32, u32, u32, u32);
u32 TextWindow_ResolveString(struct P *, u32);
void TextState_Reset(u8 *);
void TextWindow_OnPlayback(void);
s32 TextWindow_Refresh(u32 a, u32 b)
{
    struct P *p = TextWindow_Find(a);
    u8 *q;
    if (p == 0)
        return -1;
    q = p->q;
    if (p->w14 != 0) {
        TextWindow_BlankRect(p->b4, p->b5, p->b6, p->b7);
        TextState_SetBounds(q, p->b4, p->b5, p->b6, p->b7);
        p->h0e = b;
        *(u32 *)(q + 0x1c) = TextWindow_ResolveString(p, p->h0e);
        p->w5c = TextWindow_OnPlayback;
        TextState_Reset(q);
        p->h10 = 0xFFFF;
    }
    return 0;
}
