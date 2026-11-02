#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "display_animation.h"
struct PlayerSlotDisplaySlot {
    u8 resource[0x1C];
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState sequence;
    const struct ContactPosition *source;
    u16 stopOnEnd, active;
};
struct PlayerSlotDisplay {
    u8 task[0x18];
    struct PlayerSlotDisplaySlot slots[4];
    void *sequenceResource;
};
static inline u32 frame_factor(u16 v)
{
    u32 value = v;
    u32 mask = 15;
    return mask & value;
}
int PositionEffectDisplay_UpdateSources(struct PlayerSlotDisplay *m)
{
    struct SlotFrameCursor {
        u16 frame;
        u8 unknown2[10];
        struct ContactPosition position;
        u8 tail[8];
        struct DisplaySequenceState anim;
        const struct ContactPosition *source;
        u16 stopOnEnd, active;
        u8 nextFrame[0x2C];
    };
    struct PlayerSlotDisplay *owner = m;
    register struct PlayerSlotDisplay *base asm("r12") = owner;
    s32 offset = 0;
    struct SlotFrameCursor *cursor = (void *)&owner->slots[0].sprite.frame;
    struct DisplaySequenceState *sequence = &owner->slots[0].sequence;
    struct DisplaySequenceSprite *sprites = &owner->slots[0].sprite;
    do {
        if (cursor->active) {
            struct DisplaySequenceSprite *o;
            struct DisplaySequenceState *a;
            const volatile u16 *entry;
            u32 elapsed;
            int done;
            {
                u8 *address = (u8 *)&owner->slots[0].source;
                cursor->position = **(const struct ContactPosition **)(address + offset);
            }
            o = sprites;
            a = sequence;
            entry = cursor->anim.commands + cursor->anim.index;
            cursor->frame = *entry >> 6;
            {
                u32 input = a->flags;
                u32 left = 1;
                u32 mask = 1;
                left &= input;
                if (left != (((*entry & 0x30) >> 4) & mask)) {
                    register struct PlayerSlotDisplay *write asm("r2") = base;
                    write->slots[0].sprite.flags |= 4;
                } else
                    o->flags &= ~4;
            }
            {
                register u32 input asm("r0") = a->flags;
                u32 mask = 2;
                u32 left = (u8)(mask & input);
                if (left != (((*entry & 0x30) >> 4) & mask))
                    o->flags |= 8;
                else
                    o->flags &= ~8;
            }
            elapsed = ++a->elapsed;
            if ((u16)elapsed >= a->duration) {
                a->elapsed = 0;
                if (a->flags & 4) {
                    if (a->index == 0) {
                        if (a->flags & 8) {
                            a->elapsed = a->duration;
                        } else {
                            a->index = a->frameCount - 1;
                        }
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
                        } else {
                            a->index = 0;
                        }
                        done = 1;
                    } else
                        done = 0;
                }
                entry = a->commands + a->index;
                a->frameWeight = frame_factor(*entry);
                a->duration = (a->frameWeight * a->speed) >> 6;
                if (!a->duration) {
                    u32 one = 1;
                    a->duration = one;
                }
            } else
                done = 0;

            if (done && cursor->stopOnEnd) {
                {
                    u8 *address = (u8 *)&owner->slots[0].sprite;
                    {
                        struct DisplaySequenceSprite *dst = (void *)(address + offset);
                        dst->flags |= 1;
                    }
                }
                cursor->stopOnEnd = 0;
                cursor->active = 0;
            }
        }
        base = (void *)((u8 *)base + sizeof(struct PlayerSlotDisplaySlot));
        offset += sizeof(struct PlayerSlotDisplaySlot);
        cursor++;
        sequence = (void *)((u8 *)sequence + sizeof(struct PlayerSlotDisplaySlot));
        sprites = (void *)((u8 *)sprites + sizeof(struct PlayerSlotDisplaySlot));
    } while ((s32)base <= (s32)((u8 *)owner + 3 * sizeof(struct PlayerSlotDisplaySlot)));
    return 0;
}
