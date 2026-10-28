#include "global.h"
#include "area_effect_link.h"
struct G {
    u8 f[0x1c];
    u8 b;
};
struct S {
    u8 f[0xFE4];
    void *fp;
    u8 p[0x18];
    u8 st;
    u8 q[0x23];
    u8 c;
};
void SaveMenu_UpdateSlotCommit(void);
static inline void set(struct S *s, void *fp, u8 st)
{
    s->fp = fp;
    s->c = 1;
    s->st = st;
}
void SaveMenu_WaitForSaveTransition(struct S *s)
{
    struct G *g = gLinkSession;
    s32 v = -1;
    if (g != 0)
        v = g->b;
    if (v == 0x1b) {
        set(s, SaveMenu_UpdateSlotCommit, 6);
    }
}
