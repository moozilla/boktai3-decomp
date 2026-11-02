#include "global.h"

union CloudPosition {
    u32 words[2];
    struct {
        u16 x, y, z, pad;
    } coordinates;
};
struct CloudSource {
    u8 pad0[0x20];
    u32 flags;
    u8 pad24[0xC];
    union CloudPosition position;
};
struct CloudParticle {
    u8 kind;
    u8 frame;
    u8 timer;
    u8 pad3[0x1D];
    u8 sprite[0x28];
};
struct CloudTask {
    u8 pad0[0x18];
    u32 ticks;
    u16 mode;
    u8 spawnInterval;
    u8 stopFlag;
    u32 frameBase;
    union CloudPosition position;
    u32 sheet;
    struct CloudParticle particles[40];
    struct CloudSource *source;
};
extern u8 *gGameState;
extern const u16 gUnk_086126C8[];
extern const u32 gUnk_086126BC[];
void SpriteDrawable_SelectTileFrame(void *, u32, u32);
void sub_08249240(void *, void *, u32);
void Task_Kill(void *);
static inline u32 CloudParticle_NextFrame(struct CloudParticle *p)
{
    return p->frame + 1;
}
u32 sub_081D31EC(struct CloudTask *task)
{
    u8 *p = (u8 *)task;
    struct CloudParticle *e;
    s32 i;
    switch (task->mode) {
        case 1: {
            struct CloudSource *src = task->source;
            task->position = src->position;
            task->position.coordinates.y += 0x80;
        } break;
        case 2: {
            struct CloudSource *src = task->source;
            u32 bit = 0x80;
            if (src->flags & bit) {
                task->position = src->position;
                if (*(s16 *)(gGameState + 0x85e) == 0) {
                    task->position.coordinates.x += 0x14;
                    task->position.coordinates.y += 0x1e0;
                    task->position.coordinates.z -= 0x14;
                } else {
                    task->position.coordinates.x -= 0x14;
                    task->position.coordinates.y += 0x1e0;
                    task->position.coordinates.z += 0x14;
                }
                p[0x1e] = 6;
            } else {
                task->position = src->position;
                task->position.coordinates.y += 0x80;
                p[0x1e] = 4;
            }
        } break;
        case 3: {
            struct CloudSource *src = task->source;
            task->position = src->position;
            if (*(s16 *)(gGameState + 0x85e) == 0) {
                task->position.coordinates.x += 0x14;
                task->position.coordinates.y += 0x1e0;
                task->position.coordinates.z -= 0x14;
            } else {
                task->position.coordinates.x -= 0x14;
                task->position.coordinates.y += 0x1e0;
                task->position.coordinates.z += 0x14;
            }
            p[0x1e] = 4;
        } break;
    }
    e = (struct CloudParticle *)(p + 0x30);
    for (i = 39; i >= 0; i--, e++) {
        if (e->kind) {
            u32 value, mask, frame;
            u8 zero;
            value = e->timer + 1;
            zero = 0;
            e->timer = value;
            mask = 255;
            frame = e->frame;
            if ((u8)value >= gUnk_086126C8[frame & mask]) {
                e->timer = zero;
                value = CloudParticle_NextFrame(e);
                e->frame = value;
                if ((value & mask) > 1)
                    e->frame = zero;
                SpriteDrawable_SelectTileFrame(e->sprite, *(u32 *)(p + 0x2c),
                                               *(u32 *)(p + 0x20) + e->frame);
            }
            sub_08249240(p, e, gUnk_086126BC[e->kind]);
        }
    }
    if (p[0x1f])
        Task_Kill(p);
    *(u32 *)(p + 0x18) += 1;
    return 0;
}
