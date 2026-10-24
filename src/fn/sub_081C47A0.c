#include "global.h"
#include "engine.h"
struct Registry6A37 {
    u8 pad0[0x1C];
    u16 x, y;
    u8 pad20[4];
    u32 timer;
    u8 pad28[0x4C - 0x28];
    u8 phase, step;
    u8 pad4E[0x58 - 0x4E];
    u32 counter;
    s32 script;
    void *panel;
    u8 pad64[0x6C - 0x64];
    u32 active;
};
void sub_081C4708(struct Registry6A37 *);
void Registry6A37_StartFadeIn(struct Registry6A37 *);
void *RegistryF3E4_CreateHidden(s32);
// confidence=low: sets up the layers, reads keyword 'e' as the script, starts the fade-in and
// creates a hidden panel. Declared s32 with no return statement, as in the ROM.
s32 sub_081C47A0(struct Registry6A37 *p)
{
    s32 value;
    sub_081C4708(p);
    p->x = 0x15E;
    p->y = 0xC4;
    if ((value = Script_SeekToKeyword(0x65)))
        p->script = Script_GetValue();
    else
        p->script = value;
    Registry6A37_StartFadeIn(p);
    {
        u32 zero = 0;
        p->timer = zero;
        p->step = zero;
        p->phase = zero;
        p->counter = zero;
    }
    p->panel = RegistryF3E4_CreateHidden(2);
    p->active = 1;
}
