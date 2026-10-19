#define Vanargand_InitScriptedPartSprite sub_081ADDF0
#include "global.h"
struct VanargandPartAnimationData {
    u8 pad0[6];
    u16 bank;
    u8 pad8[4];
    void *position;
};
struct VanargandPartSprite {
    u32 flags;
    u8 pad4[3];
    u8 mode;
    u8 pad8[8];
    u16 priority;
    u8 pad12[0x2c-0x12];
    struct VanargandPartAnimationData animation;
    u8 pad3C[0x74-0x3c];
    void *script;
    u32 timer;
    void *coordinates;
    s16 scale[3];
    u8 pad86[2];
    u32 state;
};
s32 AnimData_Load(void *, u32);
void *ScriptProc_Start(u16, u16);
void Sprite_Init(void *, void *, u32);
static inline s32 Vanargand_HalveSigned(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}
void sub_081ADDF0(u8 *owner, struct VanargandPartSprite *part, void *coordinates)
{
    struct VanargandPartAnimationData *data = &part->animation;
    u32 packed;
    u16 zero;
    s16 *components;
    s32 x, y;
    s16 *scale;
    AnimData_Load(data, 0x89ca);
    data->bank = 0x6f;
    data->position = owner + 0x718;
    Sprite_Init(part, data, 0);
    zero = 0;
    part->priority = 0x18;
    part->mode = 2;
    part->script = ScriptProc_Start(0x922e, 0x572b);
    part->flags |= 1;
    part->coordinates = coordinates;
    part->state = zero;
    part->timer = zero;
    scale = part->scale;
    packed = zero;
    components = (s16 *)&packed;
    x = components[0];
    y = components[1];
    scale[0] = Vanargand_HalveSigned(((x + y) << 5) / 3 + 64);
    scale[1] = 64;
    scale[2] = Vanargand_HalveSigned(((y - x) << 5) / 3 + 64);
}
