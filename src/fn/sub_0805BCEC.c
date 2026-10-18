#include "scene_panel_config.h"
void ScenePanel_InitPaletteSprite(struct ScenePanel *input)
{
    struct ScenePanelConfigView *parent = (void *)input;
    if (!parent->suppressAuxiliary) {
        u32 spriteOffset = (u32) & ((struct ScenePanelConfigView *)0)->paletteSprite;
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
            u32 offset = (u32) & ((struct ScenePanelConfigView *)0)->paletteSprite.paletteCopy;
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
            u32 offset = (u32) & ((struct ScenePanelConfigView *)0)->origin;
            struct ContactPosition position;
            {
                const struct ContactPosition *origin = (void *)((u8 *)parent + offset);
                position = *origin;
            }
            positionSprite->position = position;
        }
        sprite->active = 1;
    }
}
