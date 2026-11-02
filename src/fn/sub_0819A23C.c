#include "global.h"
#include "display_animation.h"
#include "display_sprite_playback.h"
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

static inline void RunCallback(struct CannonBatteryEffect *e, void *callback)
{
    if (callback)
        Callback_InvokeActor(e, callback);
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
        RunCallback(c, c->update);
        if (!c->update)
            *(u16 *)(p + 0x1c) &= ~bit;
    }
    e = (void *)(p + 0xd68);
    for (bit = 1, i = 8; i > 0; i--, e++, bit <<= 1) {
        RunCallback(e, e->update);
        if (!e->update)
            *(u16 *)(p + 0x1e) &= ~bit;
        else
            e->finished = PlayerCue_StepSequenceEnded((void *)&e->sprite, &e->sequence);
    }
    return 0;
}
