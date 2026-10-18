#include "global.h"
#include "player.h"
struct HiddenEffectOwner;
struct HiddenEffectPosition;
void HiddenEffect_StartAtPosition(struct HiddenEffectOwner *, struct HiddenEffectPosition *, s32);
void SceneStimulus_AddTagged(s16, s16, s16, s16, u8, u16);
void Sound_PlaySong(u32);
void Player_PlayPositionEffect(struct Player *p, struct SceneVec *position, s16 strength, u32 kind,
                  struct SceneVec *spritePosition, s32 alternate, u32 song)
{
    SceneStimulus_AddTagged(position->x, position->z, strength, 0, kind, p->positionalEffectGroup);
    if (spritePosition)
        HiddenEffect_StartAtPosition((struct HiddenEffectOwner *)p, (struct HiddenEffectPosition *)spritePosition, alternate);
    if (song) Sound_PlaySong(song);
}
