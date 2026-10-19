#include "global.h"

/* tentative: adjacent-side variant of the recoil burst phase */
#define Hresvelgr_UpdateAdjacentRecoilBurstPhase sub_0819EE68

struct ReactionVector {
    s16 x, y, z, pad;
};
struct ActorReaction {
    u8 unk0[0x54];
    struct ReactionVector position;
    struct ReactionVector velocity;
    u8 unk64[0x38];
    u32 flags;
    u8 unkA0[0xF];
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
    struct ReactionVector offsets[4];
};
struct ReactionIds {
    u32 a, b;
};
extern const u32 gUnk_0824F734[2];
void Hresvelgr_SetAnimation(struct ActorReaction *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
extern const s16 gUnk_086149C4[];
void DirectionBurst_Emit(const void *, u32, u32, u32);
s32 Callback_InvokeActor(void *, void *);
static inline s16 *AddReactionVector(s16 *out, struct ReactionVector *b, struct ReactionVector *a)
{
    out[0] = b->x + a->x;
    out[1] = b->y + a->y;
    out[2] = b->z + a->z;
    return out;
}
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct ReactionVector *v, s32 angle, s32 radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
void Hresvelgr_UpdateAdjacentRecoilBurstPhase(struct ActorReaction *actor, s32 frame)
{
    struct {
        u32 ids[2];
        s16 pos[4];
    } scratch;
    u32 step;
    *(struct ReactionIds *)scratch.ids = *(const struct ReactionIds *)gUnk_0824F734;
    if (actor->pending) {
        actor->pending = 0;
        Hresvelgr_SetAnimation(actor, actor->collider194, actor->collider174,
                               scratch.ids[actor->variant], 2, 0x44);
        Sound_PlaySong(0x4D2);
    }
    {
        u32 mask = 0x600000;
        actor->flags |= mask;
    }
    if (frame == 16)
        Sound_PlaySong(0x3E2);
    step = frame - 16;
    if (step <= 4) {
        u32 angle;
        s16 *p;
        p = AddReactionVector(scratch.pos, &actor->offsets[(actor->variant - 1) & 3],
                              &actor->position);
        p[1] = actor->ground->y + 0x80;
        angle = ((actor->variant << 6) + 16) & 255;
        {
            u32 turn = step << 4;
            angle -= 32;
            turn += angle;
            DirectionBurst_Emit(p, turn & 255, 12, 128);
        }
        polar(&actor->velocity, actor->variant * 64 + 0x80, 28 - frame);
    }
    if (frame > 50)
        actor->finished = 1;
    Callback_InvokeActor(actor, actor->callback);
}
