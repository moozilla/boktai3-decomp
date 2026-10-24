#include "global.h"
#include "state_actor.h"
static inline void facing(struct StateActor *npc, short angle)
{
    npc->angle = angle;
    {
        s32 t = (s32)(angle + 0x70) >> 5;
        u32 m = 7;
        m &= t;
        npc->facing8 = m;
    }
    {
        s32 t = (s32)(angle + 0x60) >> 6;
        u32 m = 3;
        m &= t;
        npc->facing4 = m;
    }
}
void TaggedSceneNpc_PlayTaggedAnimation(void *, s32, s32, s32, s32);
s32 DisplayAnimation_TestFinished(void *);
// confidence=low: faces away, then plays tagged animations 2, a 30-frame hold, 4 and 0.
#define TaggedSceneNpc_UpdateAnimationSequence sub_081D2160
void TaggedSceneNpc_UpdateAnimationSequence(void *context, struct StateActor *p)
{
    struct StateActorTag *tag = &p->tag;
    if (p->enter) {
        p->enter = 0;
        p->done = 0;
        facing(p, p->facing4 > 1 ? 0x80 : 0xc0);
        TaggedSceneNpc_PlayTaggedAnimation(p, 0, 1, 0, 4);
    }
    switch (tag->field4) {
        case 0: {
            struct StateActorTag *t = &p->tag;
            u16 entered = t->field6;
            t->field6 = 0;
            if (entered)
                TaggedSceneNpc_PlayTaggedAnimation(p, 2, 2, 0, 4);
            if (DisplayAnimation_TestFinished((u8 *)p + 0xd4)) {
                t->field4 = 1;
                t->field6 = 1;
                t->state.word = 0;
            }
            break;
        }
        case 1: {
            struct StateActorTag *t = &p->tag;
            t->field6 = 0;
            if (tag->state.word > 30) {
                t->field4 = 2;
                t->field6 = 1;
                t->state.word = 0;
            }
            break;
        }
        case 2: {
            struct StateActorTag *t = &p->tag;
            u16 entered = t->field6;
            t->field6 = 0;
            if (entered)
                TaggedSceneNpc_PlayTaggedAnimation(p, 4, 2, 0, 4);
            if (DisplayAnimation_TestFinished((u8 *)p + 0xd4)) {
                t->field4 = 3;
                t->field6 = 1;
                t->state.word = 0;
            }
            break;
        }
        case 3: {
            struct StateActorTag *t = &p->tag;
            u16 entered = t->field6;
            t->field6 = 0;
            if (entered)
                TaggedSceneNpc_PlayTaggedAnimation(p, 0, 2, 0, 4);
            if (p->angle == p->targetAngle)
                p->done = 1;
            break;
        }
    }
}
