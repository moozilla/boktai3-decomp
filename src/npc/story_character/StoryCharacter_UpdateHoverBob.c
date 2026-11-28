#include "global.h"
#include "engine/video/render.h"
#include "engine/math/math_shift.h"
#include "npc/manager/scene_actor_lifecycle.h"

extern const s16 gUnk_086149C4[];

void StoryCharacter_UpdateHoverBob(u32 context, struct SceneActor *p, u32 ticks)
{
    u32 animation;
    s32 bob;
    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        switch (p->variant) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
                animation = 0;
                break;
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
                animation = 0;
                break;
            case 29:
                animation = 0xb;
                break;
            default:
                animation = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, animation, 1, 0, 4);
        CpuSet(gPaletteBankData + 0x5C0, (u8 *)p + 0x1CC, 0x04000008);
    }
    SceneActor_SetFlags(p, 0x400);
    bob = Math_ShiftTowardZero(gUnk_086149C4[(ticks << 2) & 0xFF] << 4, 12);
    ((struct ContactPosition *)p->drawable.sequence.position)->y += 0x80 + bob;
    if (ticks & 0x10)
        SceneActor_SetFlags(p, 0x100);
}
