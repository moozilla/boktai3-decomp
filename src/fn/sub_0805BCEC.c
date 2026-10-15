// MATCHING_CONCESSION: register-pinning
#include "scene_panel_config.h"
void ScenePanel_InitPaletteSprite(struct ScenePanel *input)
{
    register struct ScenePanelConfigView *parent asm("r6") = (void *)input;
    if (!parent->suppressAuxiliary) {
        register u32 spriteOffset asm("r2") =
            (u32) & ((struct ScenePanelConfigView *)0)->paletteSprite;
        struct ScenePanelPaletteSprite *sprite;
        struct ScenePanelPaletteSprite *positionSprite;
        struct ScenePanelPaletteGraphics *graphics;
        u16 *paletteCopy;
        u16 *destination;
        u16 *source;
        s32 i;
        sprite = (void *)((u8 *)parent + spriteOffset);
        positionSprite = sprite;
        {
            u32 offset = (u32) & ((struct ScenePanelConfigView *)0)->paletteSprite.graphics;
            graphics = (void *)((u8 *)parent + offset);
        }
        AnimData_Load(graphics, 0xC039);
        Sprite_Init(sprite, graphics, 0);
        sprite->renderMode = 3;
        {
            register u32 offset asm("r1") =
                (u32) & ((struct ScenePanelConfigView *)0)->paletteSprite.paletteCopy;
            paletteCopy = (void *)((u8 *)parent + offset);
        }
        source = graphics->palette;
        destination = paletteCopy;
        for (i = 15; i >= 0; i--)
            *destination++ = *source++;
        graphics->palette = paletteCopy;
        sprite->transform[0] = 0;
        sprite->transform[1] = 0;
        sprite->transform[2] = 0;
        {
            register u32 offset asm("r2") = (u32) & ((struct ScenePanelConfigView *)0)->origin;
            struct ContactPosition position;
            {
                register const struct ContactPosition *origin asm("r0") =
                    (void *)((u8 *)parent + offset);
                position = *origin;
            }
            positionSprite->position = position;
        }
        sprite->active = 1;
    }
}
