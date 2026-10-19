#define DragonPuppy_UpdateHitRecovery sub_0820B2AC
#define gShakeRandomTable gUnk_0203B400
#define gShakeRandomIndex gUnk_03005308
#include "global.h"
/* Tentative hit recovery: negative duration invokes defeat; positive duration
 * jitters the render position in its final 60 frames, then resumes attacks.
 * Retained fuzzy candidate, CORE 26/115. */

struct DragonPuppyPosition { u16 x, y, z, pad; };
struct DragonPuppy {
    u8 pad00[0x18];
    s32 frame;
    struct DragonPuppyPosition position;
    u8 pad24[0x40 - 0x24];
    struct DragonPuppyPosition renderPosition;
    u8 pad48[0x108 - 0x48];
    void (*update)(void);
    u8 animation[0x20];
    s16 timer;
    s16 recoveryFlag;
    u8 pad130[2];
    u8 facing;
    u8 pad133[5];
    s16 attackDelay;
    s16 recoveryDuration;
    u8 pad13c[2];
    s16 unk13e;
    u8 pad140[0xa];
    s16 disabled;
    u8 pad14c[0x170 - 0x14c];
    void *animationData;
    u8 pad174[0x186 - 0x174];
    s16 attackIndex;
};
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void DragonPuppy_HandleDefeat(void *);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void DragonPuppy_UpdateAttackTiming(void);

u32 sub_0820B2AC(void *object)
{
    u32 index;
    u16 *table;
    u32 mask, sampleMask;
    struct DragonPuppy *p = object;
    s16 *durationPointer;
    if (!p->disabled) {
        if (p->recoveryDuration < 0 && p->frame == 1)
            DragonPuppy_HandleDefeat(p);
        durationPointer = &p->recoveryDuration;
        if (p->frame > *durationPointer - 60 && *durationPointer > 0) {
            u32 zero;
            struct DragonPuppyPosition *render;
            table = gUnk_0203B400;
            index = gUnk_03005308 + 1;
            mask = 0x3ff;
            sampleMask = 7;
            index &= mask;
            {
                u32 sample = (table[index] & sampleMask) + p->position.x - 4;
                render = &p->renderPosition;
                zero = 0;
                render->x = sample;
            }
            index = (index + 1) & mask;
            p->renderPosition.y = (table[index] & sampleMask) + p->position.y - 4;
            index = (index + 1) & mask;
            gUnk_03005308 = index;
            p->renderPosition.z = (table[index] & sampleMask) + p->position.z - 4;
            {
            s32 duration = *durationPointer;
            if (p->frame > duration && duration > 0) {
                *render = p->position;
                Anim_SetSequence(p->animation, p->animationData, 3, p->facing, zero);
                p->update = DragonPuppy_UpdateAttackTiming;
                p->frame = zero;
                p->timer = p->attackDelay;
                p->attackIndex = zero;
                p->recoveryFlag = zero;
            }
            }
        }
    }
}
