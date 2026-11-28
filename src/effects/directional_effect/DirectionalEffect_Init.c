#include "player/player.h"
#include "engine/engine.h"

void Player_OnDirectionalEffectAnimationEvent(struct DirectionalSpriteEffect *);
void DirectionalEffect_Update(struct DirectionalSpriteEffect *);

void DirectionalEffect_Init(struct Player *player)
{
    struct DirectionalSprite *sprite = &player->directionalEffect[0].sprite;
    u8 *resource = player->directionalEffect[0].resourceHeader;

    if (player->actionMode == 6) {
        AnimData_Load(resource, 0x2110);
        Sprite_Init(sprite, resource, 67);
        sprite->frame = 4;
        AnimData_SelectPaletteBank(resource, 81);
        sprite->priority = 1;
        sprite->scaleY = 127;
        sprite->scaleX = 127;
        sprite->position = *(union DirectionalSpritePosition *)&player->position;
        player->directionalEffectCallback = Player_OnDirectionalEffectAnimationEvent;
    } else {
        AnimData_Load(resource, 0x8F5D);
        Sprite_Init(sprite, resource, 1);
        AnimData_SelectPaletteBank(resource, 42);
        player->directionalEffect[0].sequenceResource = ScriptProc_Start(0x922E, 0x837);
        player->directionalEffectCallback = DirectionalEffect_Update;
    }
    player->directionalEffect[0].active = 0;
}
