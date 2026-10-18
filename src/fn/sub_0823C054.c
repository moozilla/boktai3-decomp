#include "global.h"
#include "player.h"
void SceneStimulus_AddTagged(s16, s16, s16, s16, u8, u16);
void EffectQuad_StartRemoteEntry(u32, const void *);
void SpriteEffect_StartBriefAnimation(struct Player *, const void *, u32);
void Player_PlayLocalSong(s32, u32);
void Player_EmitPositionEffect(struct Player *s, const struct SceneVec *position, s32 height,
                               u32 variant, const void *effect, u32 flags, u32 song)
{
    SceneStimulus_AddTagged(position->x, position->z, (s16)height, 0, (u8)variant,
                            s->positionalEffectGroup);
    if (effect) {
        EffectQuad_StartRemoteEntry(s->slot, effect);
        SpriteEffect_StartBriefAnimation(s, effect, flags);
    }
    if (song)
        Player_PlayLocalSong(s->slot, song);
}
