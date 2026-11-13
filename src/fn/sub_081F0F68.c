#include "global.h"

#include "scene_departure_display.h"

static inline u32 StepPlayback(struct DepartureDrawable *owner, struct DisplaySequenceState *frames,
                               u32 one)
{
    struct DisplaySequenceState *state = frames;
    return PlayerCue_StepSequenceEndedWithFirstFlipMask(&owner->sprite, state, one);
}

void SingletonSlot7_InitDisplaySprites(struct DepartureActor *p)
{
    struct DepartureDrawable *foreground = &p->foreground;
    struct DepartureDrawable *accent = &p->accent;
    struct ContactSwarmGraphics *animA = &p->foregroundGraphics;
    struct DisplaySequenceState *state;
    u32 zero, one, z;
    AnimData_Load(animA, p->effectResource);
    AnimData_SelectPaletteBank(animA, 3);
    {
        const void **palA = &p->foregroundGraphics.paletteData;
        CpuSet(*palA, p->foregroundPalette, 0x10);
        *palA = p->foregroundPalette;
    }
    Sprite_Init(foreground, animA, 0x11);
    z = 0;
    one = 1;
    foreground->sprite.priority = one;
    foreground->sequenceId = z;
    state = &p->foreground.sequence;
    Anim_SetSequence(state, p->sequenceResource, 0, 0, 0);
    StepPlayback(foreground, state, one);
    {
        zero = 0;
        foreground->sprite.position.x = zero;
        foreground->sprite.position.y = zero;
        foreground->sprite.position.z = zero;
    }
    {
        struct ContactSwarmGraphics *animB = &p->accentGraphics;
        u32 accentPriority;
        struct DisplaySequenceState *accentState;
        AnimData_Load(animB, p->effectResource);
        AnimData_SelectPaletteBank(animB, 2);
        {
            const void **palB = &p->accentGraphics.paletteData;
            CpuSet(*palB, p->accentPalette, 0x10);
            *palB = p->accentPalette;
        }
        Sprite_Init(accent, animB, 0x11);
        accentPriority = 1;
        accent->sprite.priority = accentPriority;
        accent->sequenceId = 4;
        accentState = &accent->sequence;
        Anim_SetSequence(accentState, p->sequenceResource, 4, 0, zero);
        StepPlayback(accent, accentState, accentPriority);
        {
            accent->sprite.position.x = 0;
            accent->sprite.position.y = 0;
            accent->sprite.position.z = 0;
        }
    }
}
