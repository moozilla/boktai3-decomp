#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_vec.h"
struct CannonTurret {
    u8 pad00[0x40];
    struct SceneVec position;
    u8 pad48[0x108 - 0x48];
    void (*update)(void);
    u8 pad10c[0x120 - 0x10c];
    s16 effectA, effectB, effectC, fireTimer;
    u8 pad128[2];
    s16 speed;
};
extern u32 gRandomIndex;
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
struct SceneVec *Player_GetEffectivePosition(s32);
u8 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
void sub_0820843C(void);
void SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                   s32, s32, u32, s32, u32, u32);
static inline u32 RandomFrom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline void CopyPosition(struct SceneVec *out, struct SceneVec *in)
{
    *out = *in;
}
static inline void Polar(struct SceneVec *v, s32 angle, s32 length)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * length);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle] * length);
}
u32 CannonTurret_FireAimedVolley(struct CannonTurret *p)
{
    struct SceneVec position, velocity;
    struct SceneVec *pos, *vel, *player, *origin;
    u8 angle;
    s32 half, speed, range;
    u32 effectMask = 0x400000;
    u32 effectKind = 0xb6;
    if (Mod(p->fireTimer, 10) == 0 && p->fireTimer > 45) {
        pos = &position;
        origin = &p->position;
        CopyPosition(pos, origin);
        pos->y += 128;
        player = Player_GetEffectivePosition(0);
        angle = Math_Atan2(player->x - origin->x, player->z - origin->z);
        {
            const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
            asm("" : "+r"(table));
            angle = angle + (RandomFrom(table) & 31) - 16;
            half = p->speed >> 1;
            range = p->speed - half;
            speed = half;
            if (range != 0)
                speed += Mod(RandomFrom(table), range);
        }
        Polar(&velocity, angle, speed);
        vel = &velocity;
        if (Mod(p->fireTimer, 20) == 0) {
            SpriteEffectController_Create(&position, vel, 0x4384, 0xf62b, 0, 0x200, 9, effectKind,
                                          2, 0, 0, 0, p->effectA, p->effectB, effectMask,
                                          p->effectC, 0, 0);
        } else {
            SpriteEffectController_Create(&position, vel, 0x4384, 0xf62b, 0, 0x400, 9, effectKind,
                                          2, 0, 0, 0, p->effectA, p->effectB, effectMask,
                                          p->effectC, 0, 0);
        }
        Sound_PlaySong(0x51b);
    }
    if (p->fireTimer > 0)
        p->fireTimer--;
    else
        p->update = sub_0820843C;
    return 0;
}
