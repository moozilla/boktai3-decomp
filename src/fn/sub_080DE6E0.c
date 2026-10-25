#include "global.h"

#include "scene_actor_registry.h"

// confidence=low: spawns a sprite-effect burst at the actor (random offset, heading-based velocity) and returns a table entry.

// confidence=low: spawns a sprite-effect burst at the actor (random offset, heading-based velocity) and returns a table entry.

// confidence=low: spawns a sprite-effect burst at the actor (random offset, heading-based velocity) and returns a table entry.
struct Sub080DE6E0Drawable {
    u8 unk0[8];
    u16 kind;
    u8 unkA[4];
    u16 flags;
};

struct Sub080DE6E0Vel {
    s16 x, y, z;
};

extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_086061E4[][8];
void SpriteEffectController_Create(void *, const void *, s32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, s32, s32, s32, s32, s32, s32);

static inline s32 Sub080DE6E0_Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

#ifdef NONMATCHING
// fuzzy: 45.0%
// scope shape 186 (differences once low registers are set aside)
// confidence=low: spawns a sprite-effect burst at the actor (random offset, heading-based velocity) and returns a table entry.
u16 sub_080DE6E0(struct SceneActorRegistryActor *actor)
{
    struct Sub080DE6E0Drawable *sb;
    u32 flag;
    u16 type;
    struct ContactPosition pos;
    struct Sub080DE6E0Vel vel;
    struct ContactPosition scratch;
    u32 idx;
    u32 angle;
    s32 scale;
    u16 result;

    sb = (struct Sub080DE6E0Drawable *)((u8 *)actor->drawable + 0x48);
    if (sb->flags != 0)
        return 0;
    {
        u16 t = actor->flags2D4 & 0x80;
        flag = (u32)(0 - (u32)t) >> 31;
    }
    actor->flags2D0 |= 0x10000;

    type = actor->paletteOffset;
    if (type == 3 || type == 1) {
        if (sb->kind == 7 || sb->kind == 5 || sb->kind == 1) {
            const u16 *table = gUnk_0203B400;

            idx = (gUnk_03005308 + 1) & 1023;
            pos.x = (table[idx] & 63) - 48 + actor->position.x;
            idx = (idx + 1) & 1023;
            gUnk_03005308 = idx;
            pos.z = (table[idx] & 63) - 48 + actor->position.z;

            scale = 0x50;
            angle = (actor->unknown54[5] + 0x80) & 0xff;
            vel.x = Sub080DE6E0_Truncate(scale * gUnk_086149C4[(angle + 0x40) & 0xff]);
            vel.y = 0;
            vel.z = Sub080DE6E0_Truncate(scale * gUnk_086149C4[angle]);

            pos.x += vel.x;
            pos.z += vel.z;
            pos.y = actor->position.y;

            ZeroFill8(&scratch);

            if (actor->paletteOffset == 3) {
                u32 life;
                gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
                life = (table[gUnk_03005308] & 63) + 20;
                SpriteEffectController_Create(&pos, &scratch, 0x1C1A, 0xD1B8, 3, 0, 0x31, 0, 1, 0,
                                              0, life, 4, 1, 0x200004, 30, 8, 8);
            } else {
                SpriteEffectController_Create(&pos, &scratch, 0x1C1A, 0xD1B8, 0x12, 0, 0x11, 0, 1,
                                              0, 0, 0, 4, 1, 0x400008, 30, 8, 8);
            }
        }
    }
    result = gUnk_086061E4[flag][sb->kind & 7];
    return result;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080DE6E0);
#endif
