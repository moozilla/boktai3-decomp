#include "global.h"
int sub_0816B59C(void *, int);
void sub_0816D920(void *, int);
void sub_08173470(void *);
void sub_08166B7C(void *, int);
void sub_08173A7C(void *);
void Sound_PlaySong(int);
void Hud_SaveGridSelection(void *);
void sub_08173E68(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
extern u16 gUnk_03005260[];
void sub_08173D2C(u8 *p)
{
    u8 *q = p + 0x4384;
    if (sub_0816B59C(q, 0xf0)) {
        u8 *a = p + 0x1620;
        sub_0816D920(a, p[0x4387]);
        sub_08173470(p);
        sub_08166B7C(p, 2);
    }
    if (gUnk_03005260[1] & 2) {
        sub_08173A7C(p);
        Sound_PlaySong(0xde);
    } else if (1 & gUnk_03005260[1]) {
        u8 *a;
        Sound_PlaySong(0xdd);
        Hud_SaveGridSelection(q);
        *((u32 *)(p + 0x1668)) &= ~1;
        a = p + 0x1680;
        sub_0816D920(a, p[0x438a]);
        Hud_SetInteractionCallback(p, sub_08173E68, 1);
    }
}
