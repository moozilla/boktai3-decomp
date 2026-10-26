#include "global.h"
#include "engine.h"

/* tentative: adjacent-side variant of the random burst phase; the spread turns the other way */

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
    u8 unk32C[0xA];
    u16 burstCount;
    u8 unk338[8];
    struct ReactionVector offsets[2];
};
struct ReactionIds {
    u32 a, b;
};
extern const u32 gUnk_0824F734[2];
extern const u32 gUnk_0824F744[2];
extern const u16 gUnk_0203B400[];
s32 sub_08249350(s32, s32);
void Hresvelgr_SetAnimation(struct ActorReaction *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
extern const s16 gUnk_086149C4[];
void DirectionBurst_Emit(const void *, u32, u32, u32);
s32 Callback_InvokeActor(void *, void *);
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
static inline void add(struct ReactionVector *dst, struct ReactionVector *a,
                       struct ReactionVector *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
static inline int randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}
void Hresvelgr_UpdateAdjacentRandomBurstPhase(struct ActorReaction *actor, s32 frame)
{
    u32 ids[2];
    u32 angles[2];
    struct ReactionVector pos;
    struct ActorReaction *self;
    *(struct ReactionIds *)ids = *(const struct ReactionIds *)gUnk_0824F734;
    *(struct ReactionIds *)angles = *(const struct ReactionIds *)gUnk_0824F744;
    self = actor;
    if (actor->pending) {
        actor->pending = 0;
        Hresvelgr_SetAnimation(actor, actor->collider194, actor->collider174, ids[actor->variant],
                               2, 6);
        actor->burstCount = sub_08249350(randomValue() >> 3, 7) + 3;
        Sound_PlaySong(0x4D2);
    }
    {
        u32 mask = 0x600040;
        actor->flags |= mask;
    }
    if (frame == 16)
        Sound_PlaySong(0x3E2);
    if (frame > 15 && frame < actor->burstCount + 16) {
        u32 angle;
        s32 t;
        s32 count;
        s32 m = -128;
        struct ReactionVector *p;
        polar(&pos, angles[actor->variant], 256);
        p = &pos;
        add(p, &actor->position, p);
        p->y = actor->ground->y + 0x80;
        {
            u8 v = self->variant;
            count = self->burstCount;
            angle = (v << 6) + 16;
        }
        t = frame - 16;
        if (count > 1)
            DirectionBurst_Emit(p, (angle + t * m / (count - 1)) & 255, 12, 128);
        else
            DirectionBurst_Emit(p, angle & 255, 12, 128);
    }
    if (frame >= self->burstCount + 0x2E)
        actor->finished = 1;
    Callback_InvokeActor(actor, actor->callback);
}
