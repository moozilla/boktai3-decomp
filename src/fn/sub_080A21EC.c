#include "global.h"

#include "scene_actor_reaction.h"
#include "scene_actor_animation.h"

// confidence=low: directional facing update; a 2-step turn plays the matching animation sequence, then eases the direction toward the facing.

// confidence=low: directional facing update; a 2-step turn plays the matching animation sequence, then eases the direction toward the facing.

// confidence=low: directional facing update; a 2-step turn plays the matching animation sequence, then eases the direction toward the facing.
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u32 Anim_SetSequence(void *anim, void *data, u32 sequence, u32 row, u32 flags);
void Anim_SetKeyframes(void *anim, void *data, u32 index, u32 mode, u32 flags);

#ifdef NONMATCHING
// fuzzy: 14.2%
// scope shape 221 (differences once low registers are set aside)
// confidence=low: directional facing update; a 2-step turn plays the matching animation sequence, then eases the direction toward the facing.
void sub_080A21EC(struct SceneActorRegistryResetView *a)
{
    u8 *dr;
    s32 direction;
    s32 facing;
    s32 diff;
    s32 seq;
    s32 target;
    s32 row;
    s32 flags;
    s32 check;
    u16 *scale;

    if (SceneActorReaction_TakePrepared(a)) {
        a->direction = ((((a->direction + 0x60) >> 6) & 3) << 6) - 0x40;
        direction = ((a->direction + 0x60) >> 6) & 3;
        facing = *((u8 *)a + 0x3CD) >> 1;
        if (direction != facing) {
            diff = facing - direction;
            if (diff < 0)
                diff = -diff;
            if (diff == 2)
                goto turn;
        }
        a->field2B0 = 1;
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
        return;
    turn:
        dr = a->drawable;
        switch (direction) {
            case 0:
                seq = 5;
                row = 0;
                flags = 0;
                check = 1;
                break;
            case 1:
                seq = 6;
                row = 1;
                flags = 0;
                check = 1;
                break;
            case 2:
                seq = 5;
                row = 0;
                flags = 4;
                check = 0;
                break;
            default:
                seq = 6;
                row = 1;
                flags = 4;
                check = 0;
                break;
        }
        if (a->drawableKind == 0) {
            a->unknown2B7 = 5;
            Anim_SetSequence(dr + 0x48, *(void **)(dr + 0x58), 5, row, flags);
        } else {
            if (check && a->unknown2B7 == seq && !a->field2B3 && (*(u32 *)(dr + 0x28) & 4) == 0)
                goto tail;
            a->unknown2B7 = seq;
            Anim_SetKeyframes(dr + 0x20, dr, seq, 2, flags ? 6 : 4);
            *(u32 *)(dr + 0x28) &= ~4;
        }
        a->field2B3 = 0;
    }
tail:
    dr = a->drawable;
    if (*(u16 *)(dr + 0x56) == 0) {
        s32 g = a->direction;
        target = (((*((u8 *)a + 0x3CD) + 5) & 7) << 5);
        diff = g - target;
        if (diff < 0)
            diff = -diff;
        if (diff > 0x40) {
            s32 d = g - target;
            if (g < target) {
                if (d < 0)
                    d = -d;
                if (d <= 0x80)
                    a->direction = g + 0x40;
                else
                    a->direction = g - 0x40;
            } else if (g > target) {
                if (d < 0)
                    d = -d;
                if (d <= 0x80)
                    a->direction = g - 0x40;
                else
                    a->direction = g + 0x40;
            }
        } else
            a->direction = target;
    }
    if (a->flags2D4 & 0xA02F) {
        a->field2C2 = 0x40;
    } else if (a->field2C2 == 0) {
        a->field2C2 = 0x40;
    }
    scale = &a->field2C2;
    if (a->drawableKind == 0)
        Anim_SetDurationScale(dr + 0x48, *scale);
    else
        Anim_SetFrameDurationScale(dr + 0x20, *scale);
    if (a->field2B3) {
        a->field2B0 = 1;
        a->direction = ((((a->direction + 0x60) >> 6) & 3) << 6) - 0x40;
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080A21EC);
#endif
