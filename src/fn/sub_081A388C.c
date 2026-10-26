#include "global.h"

// confidence=low: updates homing effect entries, animation, trails and lifetime.

struct Types {
    u32 values[4];
};
struct Vec {
    s16 x, y, z, pad;
};
struct Animation {
    const u16 *data;
    u8 flags, count, duration, rate;
    u16 frame, unknownA, scale, timer;
};
struct Entry {
    u32 flags;
    u8 pad4[0xc];
    u16 tile;
    u8 pad12[0xa];
    struct Vec position;
    u8 pad24[8];
    struct Animation animation;
    u8 collider[0xa8];
    struct Vec velocity;
    u8 padEC[0x6c];
    u8 frame, speed, trailCount;
    u8 pad15B[3];
    u16 lifetime, initialLifetime;
    u8 turnShift, heading;
    s16 homingInterval;
    u8 pad166[2];
    void *trails[4];
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
extern u32 gRandomIndex;
extern const u32 gUnk_0824F760[4];
s32 sub_08249350(s32, s32);
void *Player_GetEffectivePosition(s32);
s32 Hresvelgr_GetHomingShotTargetAngle(void *, void *);
void Collider_SetPosition(void *, void *, s32);
void Collider_Refresh(void *);
s32 Mod(s32, s32);
void *MotionEntry_CreateConfigured(void *, s32, s32, s32, u32, s32);
void EffectEntry_Release(void *);

struct Pool {
    u8 unknown0[0x34];
    void *sequence;
    u8 unknown38[4];
    u32 allocated;
};
extern struct Pool *gUnk_0200023C;
struct Entry *EffectPool_FindIdleActiveEntry(void);
void Anim_SetSequence(void *, void *, s32, s32, s32);
void ScaledShadow_Enable(void *);
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct Vec *v, s32 angle, s32 radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
void EffectPool_AllocateEntry(struct Vec *position, s32 angle, s32 speed, s32 interval)
{
    struct Pool *pool = gUnk_0200023C;
    struct Entry *entry = EffectPool_FindIdleActiveEntry();
    if (entry) {
        pool->allocated++;
        entry->position = *position;
        entry->speed = speed;
        entry->heading = angle;
        entry->homingInterval = interval;
        polar(&entry->velocity, angle, speed);
        entry->initialLifetime = 191;
        entry->lifetime = 191;
        entry->frame = 0;
        entry->trailCount = 0;
        entry->turnShift = 16;
        Anim_SetSequence(&entry->animation, pool->sequence, 3, 0, 0);
        entry->flags &= ~0x201;
        ScaledShadow_Enable((u8 *)entry + 0xEC);
    }
}
