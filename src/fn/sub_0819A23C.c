#include "global.h"
#include "display_animation.h"
#include "display_sequence_inline.h"
struct CannonBatteryMessage {
    u32 id;
    u16 *data;
};
struct CannonBatterySprite {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[0x1a];
};
struct CannonBatteryEffect {
    void *update, *destroy;
    u8 animation[0x1c];
    struct CannonBatterySprite sprite;
    struct DisplaySequenceState sequence;
    u8 pad60[0x57];
    u8 finished;
};
s32 MessageQueue_GetMessagesById(u32, struct CannonBatteryMessage **);
void CannonBattery_CreateCannon(void *, u32, u32, u32, u32);

static inline u32 frame_factor(u16 v)
{
    return DisplaySequence_MaskCommand(v, 15);
}
static inline u32 CannonBattery_StepAnimation(struct CannonBatteryEffect *e)
{
    struct CannonBatterySprite *o = &e->sprite;
    struct DisplaySequenceState *a = &e->sequence;
    const u16 *entry;
    u32 done;
    entry = a->commands + a->index;
    o->frame = *entry >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    if ((u16)++a->elapsed >= a->duration) {
        a->elapsed = 0;
        if (a->flags & 4) {
            if (a->index == 0) {
                if (a->flags & 8)
                    a->elapsed = a->duration;
                else
                    a->index = a->frameCount - 1;
                done = 1;
            } else {
                a->index--;
                done = 0;
            }
        } else {
            if (++a->index >= a->frameCount) {
                if ((u8)(a->flags & 8)) {
                    a->elapsed = a->duration;
                    a->index = a->frameCount - 1;
                } else
                    a->index = 0;
                done = 1;
            } else
                done = 0;
        }
        entry = a->commands + a->index;
        a->frameWeight = frame_factor(*entry);
        a->duration = (a->frameWeight * a->speed) >> 6;
        if (!a->duration)
            a->duration = 1;
    } else
        done = 0;
    return done;
}

s32 CannonBattery_Update(u8 *p)
{
    struct CannonBatteryMessage *m;
    s32 n = MessageQueue_GetMessagesById(*(u32 *)(p + 0x18), &m);
    struct CannonBatteryEffect *c, *e;
    s32 i;
    u32 bit;
    for (; n > 0; n--, m++) {
        u16 *d = m->data;
        if (d[0] == 0)
            CannonBattery_CreateCannon(p, d[1], d[2], d[3], d[4]);
    }
    c = (void *)(p + 0x28);
    for (bit = 1, i = 16; i > 0; i--, c = (void *)((u8 *)c + 0xd4), bit <<= 1) {
        if (c->update)
            Callback_InvokeActor(c, c->update);
        if (!*(void *volatile *)&c->update)
            *(u16 *)(p + 0x1c) &= ~bit;
    }
    e = (void *)(p + 0xd68);
    for (bit = 1, i = 8; i > 0; i--, e++, bit <<= 1) {
        if (e->update)
            Callback_InvokeActor(e, e->update);
        if (!*(void *volatile *)&e->update)
            *(u16 *)(p + 0x1e) &= ~bit;
        else
            e->finished = CannonBattery_StepAnimation(e);
    }
    return 0;
}
