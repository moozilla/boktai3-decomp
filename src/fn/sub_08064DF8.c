#include "actor_attachment_orbit_playback.h"
#include "contact_actor.h"
#include "engine.h"

static inline const struct ActorAttachmentPartResource *
ActorAttachment_GetPartResource(struct ActorAttachmentSlotView *slot, struct ActorAttachmentManagerView *manager)
{
    switch (slot->kind) {
        case 2:
            return manager->partResource;
        case 3:
            return manager->partResource;
        default:
            return NULL;
    }
}

void ActorAttachment_TickDetachedParts(struct ActorAttachmentSlotView *slot)
{
    if (gActorAttachmentManager) {
        struct ActorAttachmentManagerView *manager = gActorAttachmentManager;
        struct ActorAttachmentPartSprite *parts = slot->parts;
        struct ContactPackedVector circle;
        s32 complete = 0;
        s32 i;
        for (i = 0; i <= 2; i++) {
            if (!((slot->partMask >> i) & 1)) {
                parts[i].flags |= 1;
                complete++;
            } else if (slot->status[i] & 1) {
                if (slot->counters[i] > 14) {
                    parts[i].flags |= 1;
                    complete++;
                } else {
                    ActorAttachment_StepOrbitPart(&parts[i], &slot->sequences[i],
                                                  ActorAttachment_GetPartResource(slot, manager));
                    {
                        struct ContactPosition *jitter = &slot->jitter[i];
                        parts[i].position.x += jitter->x + slot->offset.x;
                        parts[i].position.y += jitter->y;
                        parts[i].position.z += jitter->z + slot->offset.z;
                        parts[i].flags &= ~1;
                        jitter->y -= slot->counters[i] >> 1;
                    }
                    slot->counters[i]++;
                }
            } else if (slot->counters[i] > 0) {
                slot->counters[i]--;
            } else {
                u32 phase;
                const s16 *table;
                ActorAttachment_StepOrbitPart(&parts[i], &slot->sequences[i],
                                              ActorAttachment_GetPartResource(slot, manager));
                table = gUnk_086149C4;
                phase = slot->phase[i];
                circle.x = (table[(phase + 64) & 255] * 896) >> 16;
                circle.y = (table[phase] * 70 << 4) >> 16;
                slot->phase[i] = phase + 5;
                parts[i].position.x = slot->anchor->x + circle.x + slot->offset.x;
                parts[i].position.y = slot->anchor->y + circle.y + slot->position.z + slot->offset.y;
                parts[i].position.z = slot->anchor->z - circle.x + slot->offset.z;
                parts[i].flags &= ~1;
                ActorAttachment_BeginPartJitter(slot, i);
            }
        }
        if (complete > 2) {
            slot->flags31 = 0;
            {
                s32 id = slot->slotId;
                manager->occupiedMask &= ~(1 << id);
            }
            slot->update = NULL;
            slot->value = NULL;
            for (i = 0; i <= 2; i++) {
                if ((slot->partMask >> i) & 1) {
                    s32 j;
                    for (j = 0; j <= 2; j++) {
                        slot->status[j] = 0;
                        slot->phase[j] = 0;
                        slot->counters[j] = 0;
                        slot->thresholds[j] = 0;
                    }
                    Effect_Remove(&parts[i]);
                }
            }
            ActorAttachment_NotifySourceState(slot);
            slot->partMask = 0;
            slot->slotId = 0xFF;
        }
    }
}
