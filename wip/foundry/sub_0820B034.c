#define DragonPuppy_SpawnAttackProjectile sub_0820B034
#include "global.h"
/* Tentative attack projectile; retained fuzzy candidate, CORE 8/128. */

struct DragonPuppyPositionWords { u32 a, b; };
struct DragonPuppy {
    u8 pad00[0x40];
    s16 position[4];
    u8 pad48[0x120 - 0x48];
    s16 collision[3];
    u8 pad126[0x132 - 0x126];
    u8 facing;
    u8 pad133[3];
    s16 movement;
};
extern const s16 gUnk_086149C4[];
void ProjectileConfig_Init(void *);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetCollision(void *, u32, u32, u32, u32, u32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, u32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
void sub_08202B30(void *, void *);
void Sound_PlaySong(u32);

static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

static inline void facingOffset(s16 *out, u8 angle, s16 radius)
{
    s32 x = scale(radius * gUnk_086149C4[(angle + 0x40) & 0xff]);
    s32 zero = 0;
    out[0] = x;
    out[1] = zero;
    out[2] = scale(radius * gUnk_086149C4[angle]);
}
struct DragonPuppyVector { u16 x, y, z; };
static inline void addPosition(struct DragonPuppyVector *out, struct DragonPuppyVector *a, struct DragonPuppyVector *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
static inline void raisePosition(s16 *pos, u8 facing)
{
    if ((u8)(facing - 1) <= 1)
        pos[1] += 0xe0;
    else
        pos[1] += 0x80;
}
void sub_0820B034(struct DragonPuppy *p)
{
    u8 config[0x5c];
    s16 position[4];
    s16 offset[3];
    u32 angle;
    s32 radius;
    s32 value;
    *(struct DragonPuppyPositionWords *)position = *(struct DragonPuppyPositionWords *)p->position;
    raisePosition(position, p->facing);
    facingOffset(offset, (u8)((p->facing << 6) + 0xc0), 0xa0);
    addPosition((void *)position, (void *)position, (void *)offset);
    ProjectileConfig_Init(config);
    ProjectileConfig_SetAppearance(config, 0x7e7c, 0, 0x1b9);
    ProjectileConfig_SetCollision(config, 0, 0, p->collision[0], p->collision[1], p->collision[2]);
    ProjectileConfig_SetMovement(config, 0, 0x78, (u8)((p->facing << 6) + 0xc0), p->movement);
    ProjectileConfig_SetAnimation(config, 0xa6cf, 2, 4);
    sub_08202B30(config, position);
    Sound_PlaySong(0x520);
}
