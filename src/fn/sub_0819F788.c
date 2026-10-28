#include "global.h"
#include "engine.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: alternates side-specific sequences and emits four directional shots per pulse.
struct Vec {
    s16 x, y, z, pad;
};
struct Sequences {
    u32 ids[2];
};
struct Hresvelgr {
    u8 pad0[0x54];
    struct Vec position;
    u8 pad5C[0x40];
    u32 flags;
    u8 padA0[15];
    u8 finished, padB0, pending, pulse;
    u8 padB3[0x51];
    void *callback;
    u8 pad108[0x6c];
    u8 animationData[0x20], animation[0x20];
    u8 pad1B4[0x177];
    u8 side;
    u8 pad32C[12];
    struct Vec offsets[2];
};
extern const struct Sequences gUnk_0824F72C;
extern const u16 gUnk_0203B400[];
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void Anim_SetFrameDurationScale(void *, u32);
void Sound_PlaySong(u32);
void DirectionBurst_Emit(void *, u32, s32, s32);
s32 sub_08249350(s32, s32) __attribute__((const));
s32 Callback_InvokeActor(void *, void *);
static inline s32 RandomAt(const u16 *table, u32 index)
{
    return *(const u16 *)((index * 2) + (u32)table);
}
static inline void AddOffset(struct Vec *out, struct Hresvelgr *actor, u32 side, struct Vec *origin)
{
    struct Vec *offset = (struct Vec *)(((side << 3) + (u32)actor) + 0x338);
    out->x = origin->x + offset->x;
    out->y = origin->y + offset->y;
    out->z = origin->z + offset->z;
}
void Hresvelgr_UpdateAlternatingVolley(struct Hresvelgr *actor)
{
    struct Sequences sequences = gUnk_0824F72C;
    struct Hresvelgr *saved = actor;
    if (actor->pending) {
        actor->pending = 0;
        Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData,
                               sequences.ids[actor->side], 2, 68);
        Anim_SetFrameDurationScale(actor->animation, 8);
        gHresvelgrVolleyCount = 0;
        Sound_PlaySong(0x4d2);
    }
    {
        u32 mask = 0x600000;
        actor->flags |= mask;
    }
    if (actor->pulse) {
        struct Vec pos;
        s32 i;
        register u32 *index asm("r8");
        AddOffset(&pos, actor, actor->side, &actor->position);
        Sound_PlaySong(0x3e2);
        index = &gRandomIndex;
        i = 3;
        do {
            register const u16 *table asm("r6") = gUnk_0203B400;
            register u32 *firstIndex asm("r3") = index;
            register u32 localIndex asm("r4");
            s32 random;
            u32 firstMask;
            localIndex = *firstIndex;
            localIndex++;
            firstMask = 1023;
            asm("" : "+r"(firstMask));
            localIndex &= firstMask;
            *firstIndex = localIndex;
            random = RandomAt(table, localIndex);
            {
                s32 base = saved->side << 6;
                s32 variation = (sub_08249350(random >> 3, 9) << 4) - 64;
                register s32 angle asm("r1") = base + variation;
                register s32 speed asm("r2");
                s32 duration;
                angle &= 255;
                {
                    register u32 mask asm("r3");
                    localIndex++;
                    mask = 1023;
                    localIndex &= mask;
                }
                *index = localIndex;
                {
                    s32 sample = RandomAt(table, localIndex) >> 3;
                    register s32 residue asm("r0") = sample % 16;
                    speed = residue + 8;
                }
                {
                    register u32 next asm("r0") = localIndex + 1;
                    next &= 1023;
                    *index = next;
                    duration = (RandomAt(table, next) >> 3) % 64 + 96;
                }
                DirectionBurst_Emit(&pos, angle, speed, duration);
            }
            {
                s32 minus = -1;
                register s32 test asm("r1");
                asm("" : "+r"(minus));
                i += minus;
                test = i;
                asm("" : "+r"(test));
                if (test < 0)
                    break;
            }
        } while (1);
        if (gHresvelgrVolleyCount > 15) {
            actor->finished = 1;
            Anim_SetFrameDurationScale(saved->animation, 64);
        } else {
            Hresvelgr_SetAnimation(saved, saved->animation, saved->animationData,
                                   sequences.ids[(saved->side + gHresvelgrVolleyCount) & 1], 2, 4);
            gHresvelgrVolleyCount++;
        }
    }
    {
        u32 callbackOffset = 0x104;
        void **slot;
        void *callback;
        slot = (void **)((u8 *)actor + callbackOffset);
        callback = *slot;
        Callback_InvokeActor(actor, callback);
    }
}
