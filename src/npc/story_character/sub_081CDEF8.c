#include "global.h"
#include "engine/math/math_shift.h"
#include "npc/story_character/scene_character.h"
#include "npc/manager/scene_actor_lifecycle.h"

extern const s16 gUnk_086149C4[];

void sub_081CDEF8(void *context, struct SceneActor *actor)
{
    struct SceneCharacterEffects *effects = (struct SceneCharacterEffects *)actor->unknown20;
    if (effects->tag == 1) {
        s32 bob;
        SceneActor_SetFlags(actor, 0x400);
        bob = Math_ShiftTowardZero(gUnk_086149C4[(actor->stateFrame << 2) & 0xff] << 4, 12);
        ((struct ContactPosition *)actor->drawable.sequence.position)->y += 0x80 + bob;
        if (actor->variant == 0x1d && actor->state == 1) {
            u32 animation;
            switch (actor->variant) {
                case 4:
                    animation = 0;
                    break;
                case 9:
                    animation = 0;
                    break;
                case 0x10:
                    animation = 0;
                    break;
                case 0x14:
                    animation = 0;
                    break;
                case 0x18:
                    animation = 0;
                    break;
                case 0x1d:
                    animation = 0xb;
                    break;
                default:
                    animation = 0;
                    break;
            }
            Actor_PlayAnimationEntry(actor, animation, 1, 0, 1);
        }
    } else if (actor->state == 0x54) {
        SceneActor_SetFlags(actor, 0x400);
    }
}
