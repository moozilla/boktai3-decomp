#define SpritePairPool_Create       FollowingSpriteEffect_EnsureManager
#define SpritePairEntry_SetCallback FollowingSpriteEffect_SetStateCallback
#define gSpritePairPool             gUnk_020004D4
#include "global.h"
struct SpritePairEntry {
    u8 pad0[0x44];
    u8 animData[0x1c];
    u32 spriteFlags;
    u8 pad64[3];
    u8 spriteMode;
    u8 pad68[0x14];
    u32 bounds[2];
    u8 pad84[8];
    u8 anim[0x10];
    void *sequence;
    void *position;
    u8 padA4[0x20];
    u32 *height;
    u8 padC8[4];
    u16 duration;
    u8 active;
    u8 padCF;
    u16 id;
    u8 padD2[0xa];
};
struct SpritePairPool {
    u8 pad0[0x18];
    struct SpritePairEntry entries[2];
    u32 counter;
};
struct SpritePairParams {
    u16 unk0, duration, unk4, value;
};
extern struct SpritePairPool *gUnk_020004D4;
struct SpritePairPool *FollowingSpriteEffect_EnsureManager(void);
u8 SpritePairPool_TestUnavailable(struct SpritePairPool *, u32, s32);
void SpritePairEntry_CreateEffect(struct SpritePairEntry *, void (*)(void), u32, u32);
void FollowingSpriteEffect_SetStateCallback(struct SpritePairEntry *, void (*)(void));
void FollowingSpriteEffect_OnAnimationComplete(void);
void FollowingSpriteEffect_UpdateFollowing(void);
void Sound_PlaySong(u32);
static inline u32 SpritePairPool_MakeKey(u32 counter, u32 id)
{
    u32 mask = 0xff;
    id &= mask;
    counter &= mask;
    counter <<= 8;
    id |= counter;
    return id;
}
s32 SpritePairPool_StartEntry(void *position, u32 *height, u32 unused,
                              struct SpritePairParams *params, u32 id)
{
    struct SpritePairPool *pool;
    s32 i;
    u32 key;
    if (!gUnk_020004D4) {
        pool = FollowingSpriteEffect_EnsureManager();
        if (!pool)
            return -1;
    } else
        pool = gUnk_020004D4;
    key = pool->counter;
    key = SpritePairPool_MakeKey(key, id);
    if (SpritePairPool_TestUnavailable(pool, key, 1))
        return -1;
    for (i = 0; i < 2; i++) {
        if (!pool->entries[i].active) {
            pool->entries[i].position = position;
            pool->entries[i].height = height;
            pool->entries[i].duration = params->duration;
            pool->entries[i].active = 1;
            key = pool->counter;
            key = SpritePairPool_MakeKey(key, id);
            pool->entries[i].id = key;
            SpritePairEntry_CreateEffect(&pool->entries[i],
                                         FollowingSpriteEffect_OnAnimationComplete, params->value,
                                         id + 1);
            FollowingSpriteEffect_SetStateCallback(&pool->entries[i], FollowingSpriteEffect_UpdateFollowing);
            Sound_PlaySong(0x3e4);
            return i;
        }
    }
    return -1;
}
