#include "repeated_effect.h"
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, u32);
void sub_08225AB4(void *, u16, struct RepeatedEffectPosition *, u32, u32, struct RepeatedEffect *);
void sub_08003C00(void *);
void RepeatedEffect_InitSprite(struct RepeatedEffect *effect,
                               struct RepeatedEffectPosition *position, u32 bodyEnabled)
{
    void *graphics = effect->graphics;
    AnimData_Load(graphics, 0xA680);
    Sprite_Init(effect, graphics, 0);
    AnimData_SelectPaletteBank(graphics, 0x62);
    effect->sprite.renderMode = 3;
    effect->sprite.position = *position;
    effect->bodyRegistered = bodyEnabled;
    if ((u8)bodyEnabled) {
        void *body = effect->body;
        sub_08225AB4(body, effect->tag, &effect->sprite.position, 0, 7, effect);
        sub_08003C00(body);
    }
}
