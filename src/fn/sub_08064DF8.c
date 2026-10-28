// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_orbit_playback.h"
void ActorAttachment_TickDetachedParts(struct ActorAttachmentSlotView *slot)
{
    register struct ActorAttachmentManagerView *loaded asm("r0") = gActorAttachmentManager;
    if (loaded) {
        struct ActorAttachmentManagerView *manager = loaded;
        struct ActorAttachmentPartSprite *parts = slot->parts;
        struct ContactPackedVector circle;
        s32 complete = 0;
        s32 i = 0;
        s32 loopIndex;
        u8 *partMask = &slot->partMask;
        s32 *counterBase;
        s32 counterOffset;
        struct DisplaySequenceState *sequence;
        register struct ContactPosition *jitter asm("r10");
        register struct ActorAttachmentPartSprite *sprite asm("r6");
        {
            register s32 zero asm("r3") = 0;
            asm("" : "+r"(zero));
            counterOffset = zero;
        }
        {
            u32 start = 0x104;
            start += (u32)slot;
            sequence = (void *)start;
        }
        {
            struct ContactPosition *start = slot->jitter;
            jitter = start;
        }
        sprite = parts;
        do {
            u8 *mask = partMask;
            s32 selected;
            u32 index;
            register u32 one asm("r3");
            u32 drawFlags;
            selected = *mask;
            index = i;
            selected >>= index;
            one = 1;
            if (!(selected & one)) {
                drawFlags = sprite->flags;
                goto hide;
            } else if (slot->status[i] & 1) {
                s32 *base = slot->counters;
                register s32 current asm("r1");
                {
                    register s32 byteOffset asm("r2") = counterOffset;
                    register s32 *counter asm("r1");
                    asm("" : : "r"(byteOffset));
                    counter = (s32 *)((u8 *)base + byteOffset);
                    current = *counter;
                }
                counterBase = base;
                if (current > 14) {
                    drawFlags = sprite->flags;
                    one = 1;
                hide:
                    drawFlags |= one;
                    sprite->flags = drawFlags;
                    {
                        s32 count = complete;
                        count++;
                        complete = count;
                    }
                } else {
                    register const struct ActorAttachmentPartResource *resource asm("r12");
                    switch (slot->kind) {
                        case 2: {
                            register struct ActorAttachmentManagerView *owner asm("r1") = manager;
                            const struct ActorAttachmentPartResource *loaded;
                            asm("" : : "r"(owner));
                            loaded = owner->partResource;
                            resource = loaded;
                            break;
                        }
                        case 3: {
                            register struct ActorAttachmentManagerView *owner asm("r2") = manager;
                            const struct ActorAttachmentPartResource *loaded;
                            asm("" : : "r"(owner));
                            loaded = owner->partResource;
                            resource = loaded;
                            break;
                        }
                        default: {
                            u32 zero = 0;
                            resource = (void *)zero;
                            break;
                        }
                    }
                    ActorAttachment_StepOrbitPart(sprite, sequence, resource);
                    {
                        u32 delta = (u16)slot->offset.x;
                        register struct ContactPosition *sample asm("r3");
                        register u32 offset asm("r3");
                        u32 coordinate;
                        sample = jitter;
                        offset = (u16)sample->x;
                        delta += offset;
                        asm("" : "+r"(delta));
                        coordinate = (u16)sprite->position.x;
                        coordinate += delta;
                        sprite->position.x = coordinate;
                    }
                    {
                        register struct ContactPosition *sample asm("r5") = jitter;
                        {
                            u32 coordinate = (u16)sample->y;
                            coordinate += (u16)sprite->position.y;
                            sprite->position.y = coordinate;
                        }
                        {
                            u32 delta = (u16)slot->offset.z;
                            register u32 offset asm("r2");
                            u32 coordinate;
                            offset = (u16)sample->z;
                            delta += offset;
                            asm("" : "+r"(delta));
                            coordinate = (u16)sprite->position.z;
                            coordinate += delta;
                            sprite->position.z = coordinate;
                        }
                    }
                    sprite->flags &= ~1;
                    {
                        register s32 *base asm("r3") = counterBase;
                        register u32 offset asm("r5");
                        register s32 *counter asm("r2");
                        s32 radius;
                        struct ContactPosition *sample;
                        offset = counterOffset;
                        asm("" : : "r"(offset));
                        counter = (s32 *)((u8 *)base + offset);
                        radius = *counter >> 1;
                        sample = jitter;
                        sample->y -= radius;
                        (*counter)++;
                    }
                }
            } else {
                s32 *base = slot->counters;
                register s32 byteOffset asm("r5");
                s32 *counter;
                register s32 count asm("r0");
                byteOffset = counterOffset;
                asm("" : : "r"(byteOffset));
                counter = (s32 *)((u8 *)base + byteOffset);
                count = *counter;
                if (count > 0) {
                    count--;
                    *counter = count;
                } else {
                    register const struct ActorAttachmentPartResource *resource asm("r12");
                    u32 phase;
                    const s16 *table;
                    u8 *phasePointer;
                    switch (slot->kind) {
                        case 2: {
                            struct ActorAttachmentManagerView *owner = manager;
                            const struct ActorAttachmentPartResource *loaded;
                            loaded = owner->partResource;
                            resource = loaded;
                            break;
                        }
                        case 3: {
                            struct ActorAttachmentManagerView *owner = manager;
                            const struct ActorAttachmentPartResource *loaded;
                            loaded = owner->partResource;
                            resource = loaded;
                            break;
                        }
                        default: {
                            register u32 zero asm("r0") = 0;
                            asm("" : : "r"(zero));
                            resource = (void *)zero;
                            break;
                        }
                    }
                    ActorAttachment_StepOrbitPart(sprite, sequence, resource);
                    table = gUnk_086149C4;
                    phasePointer = slot->phase;
                    phasePointer += i;
                    phase = *phasePointer;
                    circle.x = (table[(phase + 0x40) & 0xFF] * 896) >> 16;
                    {
                        s32 sample = table[phase];
                        u32 factor;
                        factor = 70;
                        circle.y = (sample * factor << 4) >> 16;
                    }
                    phase += 5;
                    *phasePointer = phase;
                    sprite->position.x = slot->anchor->x + circle.x + slot->offset.x;
                    sprite->position.y =
                        slot->anchor->y + circle.y + slot->position.z + slot->offset.y;
                    sprite->position.z = slot->anchor->z - circle.x + slot->offset.z;
                    sprite->flags &= ~1;
                    ActorAttachment_BeginPartJitter(slot, i);
                }
            }
            {
                s32 offset = counterOffset;
                register u32 sequenceStep asm("r3");
                u32 jitterStep;
                u32 one;
                offset += 4;
                counterOffset = offset;
                sequenceStep = 16;
                asm("" : "+r"(sequenceStep));
                sequence = (void *)((u32)sequence + sequenceStep);
                jitterStep = 8;
                jitter = (void *)((u32)jitter + jitterStep);
                sprite++;
                one = 1;
                i += one;
                loopIndex = i;
            }
        } while (loopIndex <= 2);
        {
            s32 count = complete;
            if (count > 2) {
                u32 zero;
                u8 *flags31;
                s8 *slotId;
                s8 *savedSlotId;
                s32 postIndex;
                flags31 = &slot->flags31;
                zero = 0;
                *flags31 = zero;
                slotId = &slot->slotId;
                {
                    s32 id = *slotId;
                    struct ActorAttachmentManagerView *owner;
                    u32 *occupied;
                    u32 bit;
                    owner = manager;
                    occupied = &owner->occupiedMask;
                    bit = 1 << id;
                    *occupied &= ~bit;
                }
                slot->update = (void *)zero;
                slot->value = (void *)zero;
                i = zero;
                savedSlotId = slotId;
                do {
                    u8 *mask = partMask;
                    s32 selected;
                    register s32 index asm("r3");
                    register s32 next asm("r8");
                    selected = *mask;
                    index = i;
                    selected >>= index;
                    selected &= 1;
                    {
                        s32 step = 1;
                        step += i;
                        next = step;
                    }
                    if (selected) {
                        s32 j = 0;
                        register u32 byteOffset asm("r12");
                        u8 *status;
                        u32 zero;
                        u8 *phases;
                        s16 *thresholds;
                        s32 *counters;
                        index <<= 2;
                        byteOffset = index;
                        status = slot->status;
                        zero = 0;
                        phases = slot->phase;
                        thresholds = slot->thresholds;
                        counters = slot->counters;
                        do {
                            status[j] = zero;
                            phases[j] = zero;
                            *counters++ = zero;
                            *thresholds++ = zero;
                            j++;
                        } while (j <= 2);
                        {
                            u32 offset = byteOffset;
                            offset += i;
                            offset <<= 3;
                            {
                                struct ActorAttachmentPartSprite *base = parts;
                                Effect_Remove((void *)((u32)base + offset));
                            }
                        }
                    }
                    i = next;
                    postIndex = i;
                } while (postIndex <= 2);
                ActorAttachment_NotifySourceState(slot);
                {
                    u32 zero = 0;
                    register u8 *mask asm("r3");
                    mask = partMask;
                    *mask = zero;
                }
                {
                    u32 marker = 0xFF;
                    s8 *destination;
                    destination = savedSlotId;
                    *destination = marker;
                }
            }
        }
    }
}
