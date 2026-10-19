#include "global.h"
struct VanargandBoss {
    u8 unk0[0x9c];
    u32 flags;
    u8 unka0[0x522];
    s16 state;
};
struct VanargandPartSprite {
    u8 unk0[6];
    u16 animation;
    u32 unk8;
    void *animationData;
};
struct VanargandPartCollider {
    u8 unk0[6];
    u16 flags;
    u8 unk8[0x4c];
};
struct VanargandPartTarget {
    u32 unk0;
    u32 flags;
    u8 unk8[8];
};
struct VanargandPart {
    u8 unk0[0x1c];
    s16 position[3];
    u8 unk22[10];
    struct VanargandPartSprite sprite;
    u8 unk3c[0x6c];
    struct VanargandPartCollider collider;
    s16 timerFC, timerFE;
    u32 flags;
    u8 unk104[0xc];
    struct VanargandPartTarget target;
    s16 state;
    u8 unk122[6];
    u16 pendingDamage;
    u16 unk12a;
    u16 paletteTimer;
    u16 disableTimer;
};
void Collider_SetPosition(void *, void *, u32);
void Vanargand_UpdatePartCollision(struct VanargandBoss *boss, struct VanargandPart *part)
{
    struct VanargandBoss *owner = boss;
    struct VanargandPartCollider *collider;
    u32 mask = 4;
    if ((boss->flags & mask) || boss->state == 1) {
        part->flags |= 1;
        part->timerFC = 0;
        part->timerFE = 0;
    }
    if (part->disableTimer) {
        part->disableTimer--;
        part->flags |= 1;
    }
    if (part->paletteTimer) {
        if (--part->paletteTimer == 0) {
            struct VanargandPartSprite *sprite = &part->sprite;
            sprite->animation = 0x72;
            sprite->animationData = (u8 *)owner + 0x778;
        }
    }
    {
        u32 mask = 0x40;
        if (boss->flags & mask) {
            struct VanargandPartTarget *target = &part->target;
            s32 mask = -2;
            target->flags &= mask;
        } else {
            struct VanargandPartTarget *target = &part->target;
            u32 mask = 1;
            target->flags |= mask;
        }
    }
    collider = &part->collider;
    if (part->flags & 1) {
        part->pendingDamage = 0;
        collider->flags |= 4;
        if (part->state == 0) {
            struct VanargandPartTarget *target = &part->target;
            target->flags |= 1;
        }
    } else {
        s32 mask = -5;
        collider->flags = mask & collider->flags;
        Collider_SetPosition(collider, part->position, 0);
    }
}
