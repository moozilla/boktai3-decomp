#define Armor_UpdatePositiveTurn     sub_08182CDC
#define gArmorPositiveTurnAnimations gUnk_0824EBC4
#include "global.h"

// Tentative Armor family: turn by positive quarter turns until facing matches target.
// Explicit subobject views retain the original base-plus-field addressing.

struct ArmorSpriteState {
    u16 unknown0, unknown2, unknown4, flags;
};
struct ArmorControlState {
    u32 unknown0, flags;
};
struct ArmorTurnController {
    u8 pad000[0xC4];
    u16 turnDuration;
    u8 pad0C6[0x180 - 0xC6];
    struct ArmorControlState control;
    u8 pad188[8];
    struct ArmorSpriteState sprite190;
    u8 pad198[0x1E4 - 0x198];
    struct ArmorSpriteState sprite1E4;
    u8 pad1EC[0x238 - 0x1EC];
    struct ArmorSpriteState sprite238;
    u8 pad240[0x28C - 0x240];
    struct ArmorSpriteState sprite28C;
    u8 pad294[0xB32 - 0x294];
    u16 flagsB32;
    u8 padB34[5];
    u8 facing;
    u8 targetFacing;
    u8 padB3B[5];
    u8 turnState;
    u8 turnComplete;
    u8 padB42;
    u8 animationComplete;
};
extern const u32 gArmorPositiveTurnAnimations[];
void Armor_SetAnimationFromStart(u32, u32);

void sub_08182CDC(struct ArmorTurnController *p, s32 duration)
{
    u8 *s = &p->turnState;
    u8 state = *s;
    switch (state) {
        case 0: {
            u32 four = 4;
            if (p->facing == p->targetFacing) {
                *s = 0xFF;
                p->turnComplete = 1;
                break;
            }
            Armor_SetAnimationFromStart(
                (u32)p, (u32)&gArmorPositiveTurnAnimations[((p->facing + 0x60) >> 6) & 3]);
            (*s)++;
            p->turnComplete = state;
            p->turnDuration = duration;
            ((struct ArmorSpriteState *)((u8 *)p + 0x238))->flags &= ~four;
            ((struct ArmorSpriteState *)((u8 *)p + 0x1E4))->flags &= ~four;
            ((struct ArmorSpriteState *)((u8 *)p + 0x28C))->flags &= ~four;
            ((struct ArmorSpriteState *)((u8 *)p + 0x190))->flags |= four;
            {
                u32 one = 1;
                ((struct ArmorControlState *)((u8 *)p + 0x180))->flags &= ~one;
            }
            p->flagsB32 &= ~0x20;
            break;
        }
        case 1:
            if (p->animationComplete != 0) {
                p->facing += 0x40;
                if (p->facing != p->targetFacing) {
                    *s = 0;
                } else {
                    *s = 0xFF;
                    p->turnComplete = state;
                }
            }
            break;
    }
}
