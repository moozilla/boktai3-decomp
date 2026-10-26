// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define FrostEmitter_Initialize sub_081D3094
struct FrostVec {
    u16 x, y, z, pad;
};
struct FrostSequence {
    const u16 *commands;
    u8 flags, count, weight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct FrostParticle {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[10];
    struct FrostVec position;
    u8 unknown24[8];
    struct FrostSequence sequence;
    struct FrostVec velocity;
    u16 active, unknown46;
};
struct FrostEmitter {
    u8 unknown0[0x18];
    u8 graphics[0x1C];
    void *resource;
    struct FrostVec *source;
    u32 emitting;
    struct FrostParticle particles[12];
    u32 enabled, age;
};
extern u32 gRandomIndex;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void Anim_SetSequence(void *, void *, u16, u8, u8);
void AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, s32);
void *ScriptProc_Start(u32, u16);
void Sprite_Init(void *, void *, u32);
s32 FrostEmitter_Initialize(struct FrostEmitter *p)
{
    void *graphics = p->graphics;
    void *spriteGraphics;
    struct FrostParticle *slot;
    u16 *active;
    u32 *enabled;
    register u32 *age asm("sl");
    register u32 zero asm("r0");
    u32 one;
    s32 i;
    AnimData_Load(graphics, 0x4384);
    AnimData_SelectPaletteBank(graphics, 0xB6);
    p->resource = ScriptProc_Start(0x922E, 0xF62B);
    if (!p->resource)
        return -1;
    i = 0;
    spriteGraphics = graphics;
    enabled = &p->enabled;
    {
        u32 offset = 0x3A4;
        age = (void *)(offset + (u32)p);
    }
    one = 1;
    active = &p->particles[0].active;
    slot = p->particles;
    do {
        u32 flags;
        Sprite_Init(slot, spriteGraphics, 0);
        flags = slot->flags | one;
        slot->flags = flags;
        if (i & one)
            flags |= 0x200;
        else
            flags |= 0x400;
        slot->flags = flags;
        zero = 0;
        *active = zero;
        slot->priority = one;
        active += 0x24;
        slot++;
        i++;
    } while (i <= 11);
    *enabled = zero;
    *age = zero;
    return zero;
}
