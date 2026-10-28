#include "global.h"
#include "player.h"
struct Armor {
    u8 pad0[0xd4];
    u16 position[3];
    u8 padDA[0xb26 - 0xda];
    s16 projectileParam;
    u8 padB28[0xb39 - 0xb28];
    u8 direction;
};
struct ArmorProjectileOffset {
    u16 x, y, z, pad;
};
extern const struct ArmorProjectileOffset gUnk_0824EFDC[];
void ActorEffect_Spawn(u16 *, void *, s32, s32, s32, s32, s32, s32, s32, s32);
void Armor_SpawnPairedProjectiles(struct Armor *p)
{
    u8 *owner = (*(u8 **)gPlayers);
    s32 direction = p->direction >> 6;
    s32 i = 0;
    u16 pos[3];
    u16 *out = pos;
    u16 *base = p->position;
    s32 offsetBytes = direction << 3;
    for (; i < 2; i++) {
        const struct ArmorProjectileOffset *offset =
            (const struct ArmorProjectileOffset *)((const u8 *)gUnk_0824EFDC + offsetBytes);
        out[0] = base[0] + offset->x;
        out[1] = base[1] + offset->y;
        out[2] = base[2] + offset->z;
        ActorEffect_Spawn(pos, owner + 0x30, p->projectileParam, 20, 0x40000, 30, 20, 120, 240,
                          ((i << 6) + p->direction - 32) & 255);
        offsetBytes += 8;
    }
}
