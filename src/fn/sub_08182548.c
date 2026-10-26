#define gRandomValues       gUnk_0203B400
#define gArmorScatterOffset gUnk_0824EB74
#include "global.h"
#include "engine.h"
struct ArmorActor {
    u8 pad0[0x48];
    u8 position[12];
};
extern const u16 gUnk_0203B400[];
extern const s32 gUnk_0824EB74[];
void SpriteEffectController_Create(void *, const void *, s32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, s32, s32, s32, s32, s32, s32);
void Armor_SpawnScatterEffect(struct ArmorActor *s)
{
    u32 lifetime;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    lifetime = *(const u16 *)((gRandomIndex << 1) + (u32)gUnk_0203B400);
    lifetime &= 0x3f;
    lifetime += 0x14;
    SpriteEffectController_Create(s->position, gUnk_0824EB74, 0x1c1a, 0xd1b8, 3, 0, 0x31, 0, 1, 0,
                                  0, lifetime, 4, 1, 0x200004, 30, 8, 8);
}
