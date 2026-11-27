#include "player/effects/orbit_effect.h"
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
void AnimSprite_InitLinked(void *, void *, int, int, int, int, int, void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void PlayerTransition_LoadAnimation(struct PlayerOrbitEffect *p)
{
    struct AnimSpriteResource *resource = ScriptProc_Start(0xcb05, 0xde23);
    if (resource) {
        p->animationData = *resource;
        ResourceHeader_Relocate(&p->animationData, resource);
        AnimSprite_InitLinked(&p->drawable, &p->animationData, 0, 0x9000, 2, 0, 0x3c, &p->owner->position);
        Anim_SetKeyframes(&p->drawable, &p->animationData, 0xe4, 1, 0);
        p->drawable.paletteBank = 0x33;
        p->drawable.palette = p->owner->animation.palette;
    }
}
