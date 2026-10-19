#define Ratatosk_UpdateOrbitingParticles sub_081F55B0
#define Ratatosk_InitParticleParams      sub_081FBDE0
#define Ratatosk_SpawnParticle           sub_081FBBA8
#define gSineTable                       gUnk_086149C4
#include "global.h"
struct RatatoskPosition {
    s16 x, y, z, pad;
};
struct Ratatosk {
    u8 pad0[0x90];
    u32 timer;
    u8 pad94[8];
    u32 flags;
    u8 padA0[15];
    u8 complete;
    u8 padB0;
    u8 entering;
    u8 padB2[0x104 - 0xb2];
    void *callback;
    u8 pad108[0x32c - 0x108];
    u8 animData[0x20];
    u8 anim[0x20];
    struct RatatoskPosition position;
};
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
void Ratatosk_ConfigureParticle(u8 *, u16, u32, u8, u32, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
s32 sub_0824923C(void *, void *);
void sub_081FBDE0(void *);
void sub_081FBBA8(void *, void *);
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -(-v >> 12);
}
static inline u32 makeOffset(s32 radius, s32 angle, s16 *dst)
{
    dst[0] = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    dst[1] = 0;
    dst[2] = trunc12(gUnk_086149C4[angle] * radius);
    return 0;
}
static inline void addOffset(s16 *dst, s16 *src)
{
    u16 x = src[0];
    dst[0] = x + dst[0];
    dst[1] = dst[1] + src[1];
    dst[2] = src[2] + dst[2];
}
void sub_081F55B0(struct Ratatosk *p)
{
    u8 params[0x18];
    s16 offset[4];
    s16 position[4];
    u32 *timer;
    s32 angle, radius;
    u32 zero;
    if (p->entering) {
        p->entering = 0;
        SceneActors_StartUnmirroredAnimation(p, p->anim, p->animData, 0x1f, 1, 4);
        Sound_PlaySong(0x56b);
    }
    *(struct RatatoskPosition *)position = p->position;
    timer = &p->timer;
    angle = (u8)(Mod(*timer, 31) << 3);
    radius = 160;
    zero = makeOffset(radius, angle, offset);
    addOffset(position, offset);
    position[1] += 256;
    sub_081FBDE0(params);
    Ratatosk_ConfigureParticle(params, 20, 0, 0, 2, zero);
    sub_081FBBA8(params, position);
    if (*timer > 5) {
        u32 mask = 0x800000;
        p->flags |= mask;
    }
    if (*timer > 120) {
        Sound_StopSong(0x56b);
        p->complete = 1;
    }
    sub_0824923C(p, p->callback);
}
