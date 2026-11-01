#include "global.h"
#include "display_animation.h"
#include "ram.h"
#include "player_cue_sequence.h"
struct ProximityActor {
    struct DisplaySequenceSprite sprite;
    u8 pad[0x90 - sizeof(struct DisplaySequenceSprite)];
    struct DisplaySequenceState seq;
    u8 padA0[0xEB - 0xA0];
    u8 hit;
    u8 padEC[0x10];
    s16 health, maxHealth;
    s32 divisor;
    u8 pad104[2];
    u16 unk106;
    u32 state;
    u8 unk10C, unk10D;
};
struct Context {
    u8 pad[0x18];
    void *resource;
};
void Anim_RestartSequence(struct DisplaySequenceState *, void *, u32, u32, u32);
s32 Div(s32, s32);
static inline u8 Test(struct ProximityActor *p, u32 mask)
{
    if (p->state & mask)
        return 1;
    return 0;
}
static inline void Step(struct DisplaySequenceSprite *sp, struct DisplaySequenceState *st)
{
    PlayerCue_StepSequence(sp, st);
}
// confidence=low: health-derived animation and recovery; actor identity remains tentative.
void ProximityActor_UpdateDamageAnimation(struct ProximityActor *p)
{
    struct Context *ctx = gRegionActorPool;
    s32 frame;
    if (!ctx)
        return;
    if ((p->hit & 1) || Test(p, 4)) {
        if (p->health <= 0) {
            p->state = 0x28;
            Anim_SetSequence(&p->seq, ctx->resource, 1, 0, 0);
            Step(&p->sprite, &p->seq);
            p->unk106 = 3;
            p->unk10C = 1;
            p->hit = 0;
            return;
        } else {
            u32 test = Test(p, 1);
            if (test) {
                if (p->seq.index != 3) {
                    frame = p->seq.index;
                    if (frame > 3)
                        frame = __builtin_abs(frame - 6);
                    Anim_RestartSequence(&p->seq, ctx->resource, 0, 0, 0);
                    p->seq.index = frame;
                }
                p->state = 0x22;
                return;
            } else {
                p->state = 0x24;
                frame = Div(p->health, p->divisor);
                frame = 3 - frame;
                Anim_SetSequence(&p->seq, ctx->resource, 1, 0, test);
                if (p->seq.index < frame)
                    Step(&p->sprite, &p->seq);
            }
        }
    }
    if (!(p->hit & 1)) {
        if (p->health < p->maxHealth)
            p->health += 10;
        else if (Test(p, 4)) {
            Anim_SetSequence(&p->seq, ctx->resource, 1, 0, 4);
            if (Test(p, 4)) {
                frame = Div(p->health, p->divisor);
                p->seq.index = 3 - frame;
            }
            p->health = p->maxHealth;
            p->state = 0x30;
        }
        p->unk10D = 0;
    }
    p->hit = 0;
}
