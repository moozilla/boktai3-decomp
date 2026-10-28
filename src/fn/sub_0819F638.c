#include "global.h"
#include "engine.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
struct ReactionVector {
    s16 x, y, z, pad;
};
struct ActorReaction {
    u8 unk0[0x54];
    struct ReactionVector position;
    u8 unk5C[0x40];
    u32 flags;
    u8 unkA0[4];
    u32 timer;
    u8 unkA8[7];
    u8 finished;
    u8 unkB0;
    u8 pending;
    u8 unkB2[0x52];
    void *callback;
    u8 unk108[8];
    struct ReactionVector *ground;
    u8 unk114[0x60];
    u8 collider174[0x20];
    u8 collider194[0x20];
    u8 unk1B4[0x177];
    u8 variant;
    u8 unk32C[0xC];
    struct ReactionVector offsets[2];
};
struct ReactionIds {
    u32 a, b;
};
void DirectionBurst_EmitOffset(const void *, u32, u32, u32, u32, u32);
void Hresvelgr_SetAnimation(struct ActorReaction *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);

s32 Callback_InvokeActor(void *, void *);
static inline void AddReactionVector(struct ReactionVector *out, struct ReactionVector *a,
                                     struct ReactionVector *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
static inline s32 RandomReactionValue(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
void Hresvelgr_UpdateRandomVolleyPhase(struct ActorReaction *actor, s32 frame)
{
    struct ReactionVector pos;
    if (actor->pending) {
        actor->pending = 0;
        Hresvelgr_SetAnimation(actor, actor->collider194, actor->collider174, 18, 2, 4);
        gHresvelgrVolleyState = 0;
        Sound_PlaySong(0x4D2);
        actor->timer = 0;
    }
    {
        u32 mask = 0x600000;
        actor->flags |= mask;
    }
    if (frame == 16)
        Sound_PlaySong(0x3E2);
    if (frame > 15) {
        if ((actor->timer & 3) == 0) {
            s32 a;
            s32 b;
            const u16 *table;
            frame = actor->timer >> 2;
            AddReactionVector(&pos, &actor->position, &actor->offsets[actor->variant]);
            pos.y = actor->ground->y + 128;
            table = gRandomTable;
            asm("" : "+r"(table));
            a = (RandomReactionValue(table) >> 3) % 32 + 64;
            b = (RandomReactionValue(table) >> 3) % 64 + 96;
            DirectionBurst_EmitOffset(&pos, frame, actor->variant, a, frame + 12, b);
            if (frame == 4)
                actor->finished = 1;
        }
        actor->timer++;
    }
    Callback_InvokeActor(actor, actor->callback);
}
