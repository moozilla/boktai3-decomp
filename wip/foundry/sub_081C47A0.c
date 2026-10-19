#include "global.h"
// Provisional object-family interpretation.
#define SceneTransition_InitializeFromScript sub_081C47A0
struct SceneTransition {
    u8 pad0[0x1c]; s16 x, y; u8 pad20[4]; u32 tween;
    u8 pad28[0x4c-0x28]; u8 phase, state; u8 pad4e[0x58-0x4e];
    s32 counter, scriptValue; u32 target; void (*callback)(void); s32 timer, active;
};
void sub_081C4708(struct SceneTransition *);
void sub_081C44E0(struct SceneTransition *);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u32 sub_081C3FBC(s32);
u32 sub_081C47A0(struct SceneTransition *p)
{
    s32 value;
    u32 target;
    sub_081C4708(p);
    p->x = 0x15e;
    p->y = 0xc4;
    value = Script_SeekToKeyword(0x65);
    if (value) value = Script_GetValue();
    p->scriptValue = value;
    sub_081C44E0(p);
    { s32 zero = 0;
    *(u32 *)((u8 *)p + 0x24) = zero;
    *(u8 *)((u8 *)p + 0x4d) = zero;
    *(u8 *)((u8 *)p + 0x4c) = zero;
    *(u32 *)((u8 *)p + 0x58) = zero; }
    target = sub_081C3FBC(2);
    p->target = target;
    p->active = 1;

}
