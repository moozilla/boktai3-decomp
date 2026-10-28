#include "global.h"
#include "input.h"
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
void Serial_StopCommunication(void);
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, u32);
void Sound_PlaySong(s32);
void System_ShutdownAndSoftReset(void);
void SaveMenu_UpdateSaveFailure(struct S *s)
{
    if (s->c24 != 0) {
        Serial_StopCommunication();
        TextWindow_SetScript(s->h, s->c);
        TextWindow_Refresh(s->h, 3);
        Sound_PlaySong(0x192);
        s->t = 0;
        s->c24 = 0;
    }
    if (s->t <= 0x3b) {
        s->t++;
    } else if (gPadState[1] & 0x30F) {
        System_ShutdownAndSoftReset();
    }
}
