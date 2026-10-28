#define PlayerOrbitEffect_SetUpdateCallback PlayerBurst_SetStateCallback
#include "global.h"
#include "player.h"
#include "render.h"
struct PlayerOrbitEffect {
    u8 unknown0[0x1C];
    u8 animationData[0x20];
    u8 animation[0x1A];
    u8 animationState;
    u8 unknown57[7];
    s16 height;
    u8 unknown60[0x92];
    u16 frame;
};
void Sound_PlaySong(u32);
s32 Anim_Step(void *, void *);
void PlayerOrbitEffect_UpdateStatTransfer(void);
void PlayerBurst_SetStateCallback(void *, void (*)(void));
void PlayerOrbitEffect_UpdateIntro(struct PlayerOrbitEffect *effect)
{
    struct Player *player;
    if (effect->frame == 0) {
        u32 mask = 4;
        gEnginePauseFlags |= mask;
        Sound_PlaySong(0x302);
        Sound_PlaySong(0x1FA);
    }
    player = gPlayers[0];
    player->animation.flags |= 1;
    effect->height = (effect->height * 15 + player->position.y + 150) >> 4;
    Anim_Step(effect->animation, effect->animationData);
    effect->frame++;
    if (effect->frame > 59) {
        effect->animationState = 1;
        PlayerBurst_SetStateCallback(effect, PlayerOrbitEffect_UpdateStatTransfer);
    }
}
