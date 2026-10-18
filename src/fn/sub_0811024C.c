#include "global.h"
struct S {
    u8 f[0xFE4];
    void *fp;
    u32 a;
    u32 b[2];
    u32 c;
    u32 d[2];
    u8 st;
    u8 pad[7];
    u32 h;
    u8 q[0x18];
    u8 c24;
    u8 pad2;
    u16 t;
};
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, u32);
void TextWindow_StartPlayback(u32);
s32 Save_CopySelectedStateToAlternateSlot(void);
void Registry1745_RequestHandshakeContinue(void);
void SaveMenu_UpdateSaveFailure(void);
void SaveMenu_WaitForSaveTransition(void);
static inline void set(struct S *s, void *fp, u8 st)
{
    s->fp = fp;
    s->c24 = 1;
    s->st = st;
}
void SaveMenu_UpdateSaveDelay(struct S *s)
{
    if (s->c24 != 0) {
        TextWindow_SetScript(s->h, s->c);
        TextWindow_Refresh(s->h, 4);
        TextWindow_StartPlayback(s->h);
        s->t = 0;
        s->c24 = 0;
    }
    if (s->t == 0x3c) {
        if (!(u8)Save_CopySelectedStateToAlternateSlot()) {
            set(s, SaveMenu_UpdateSaveFailure, 8);
        } else {
            Registry1745_RequestHandshakeContinue();
            set(s, SaveMenu_WaitForSaveTransition, 5);
        }
    } else {
        s->t++;
    }
}
