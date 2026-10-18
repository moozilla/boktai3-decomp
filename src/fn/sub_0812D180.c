#include "global.h"
#define SceneSlot_UpdateTimedPulse sub_0812D180
#define SceneSlot_EmitPulse sub_0812CF9C
struct SceneSlot { u8 unknown[0x1c]; u8 pulse[0x7a]; u8 state; s8 index; u16 duration; u8 pad9a[2]; u16 timer; u8 pad9e; u8 reset; };
s32 Mod(s32, s32);
void sub_0812CF9C(void *);
void sub_0812D180(void *unused, struct SceneSlot *slot)
{
    if (slot->reset) { slot->timer = 0; slot->reset = 0; }
    if (Mod(slot->timer, 6) == 0) sub_0812CF9C(slot->pulse);
    if (slot->timer > slot->duration) { slot->state = 2; slot->reset = 1; }
    else slot->timer++;
}
