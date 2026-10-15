#include "staged_effect.h"
#define StagedEffect_Initialize sub_0804DD48
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void sub_08020CD4(void *, u32, u32);
u32 sub_0804DD44(void);
u32 sub_0821ABA8(u32, u32);
void sub_082151E4(void *, u32);
void sub_082144A4(void *, void *, u32);
extern u8 *gUnk_02000710;
s32 StagedEffect_Initialize(struct StagedEffect *effect, u32 script)
{
    s32 hide;
    s32 i;
    u16 *paletteIndex;
    s16 *travel;
    u16 *savedPaletteIndex;
    s16 *savedTravel;
    u32 hasScript = Script_SeekToKeyword(0x4F);
    if (hasScript)
        effect->hasScript = Script_GetValue();
    else
        effect->hasScript = hasScript;
    if (effect->hasScript) {
        sub_08020CD4((u8 *)effect + 0x18, script, 10);
        {
            u32 state = 0;
            u16 *destination = &effect->state;
            *destination = state;
        }
        effect->frame = 0;
        hide = 1;
    } else {
        u32 state = 1;
        u16 *destination = &effect->state;
        *destination = state;
        effect->frame = 0;
        hide = 0;
        sub_0822B2F8(0x1C6);
    }
    if (Script_SeekToKeyword(0x70)) {
        effect->position.axes.x = Script_GetValue();
        effect->position.axes.y = Script_GetValue();
        effect->position.axes.z = Script_GetValue();
    } else {
        u8 *game = gUnk_02000710;
        effect->position.axes.x = *(u16 *)(game + 0x30);
        effect->position.axes.y = *(u16 *)(game + 0x32);
        effect->position.axes.z = *(u16 *)(game + 0x34);
    }
    {
        u32 value = sub_0804DD44();
        paletteIndex = &effect->paletteIndex;
        *paletteIndex = value;
    }
    {
        u32 value = sub_0821ABA8(0x68, 1024);
        travel = &effect->travel;
        *travel = value;
    }
    effect->initialTravel = *travel;
    effect->spread = sub_0821ABA8(0x6D, 512);
    effect->initialSpread = effect->spread;
    effect->scale = 2032;
    {
        u32 found = Script_SeekToKeyword(0x65);
        savedPaletteIndex = paletteIndex;
        savedTravel = travel;
        if (found) {
            effect->pendingScript = (void *)Script_GetValue();
            effect->scriptReleaseFrame = Script_GetValue();
        } else {
            effect->pendingScript = 0;
            effect->scriptReleaseFrame = 0;
        }
    }
    sub_082151E4(&effect->graphics, 0x2499);
    i = 0;
    do {
        struct StagedEffectSprite *sprite = &effect->sprites[i];
        u32 xy, z;
        sub_082144A4(sprite, &effect->graphics, 2);
        sprite->priority = *savedPaletteIndex * 7;
        xy = effect->position.words[0];
        z = effect->position.words[1];
        *(u32 *)&sprite->x = xy;
        *(u32 *)&sprite->z = z;
        if (i == 0) {
            sprite->y += *savedTravel;
            sprite->flags |= 1;
        } else {
            sprite->flags |= 0x200 | hide;
        }
        sprite->renderMode = 1;
        i++;
    } while (i <= 4);
    CpuSet(gUnk_030042E4 + 0x28D0, effect->blendedPalette, 0x4000008);
    effect->activePalette = effect->blendedPalette;
    effect->graphics.view.palette = effect->blendedPalette;
    return 0;
}
