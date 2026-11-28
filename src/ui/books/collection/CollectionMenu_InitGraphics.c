#include "global.h"
#include "engine/video/display_sprite_playback.h"
#include "ui/books/collection.h"

extern const u32 gUnk_0824FEB4[30];
extern const u32 gUnk_0824FF2C[27];

void *memcpy(void *, const void *, u32);

/* Tentative: builds the collection menu widgets and the 27 entry sprites. */
void CollectionMenu_InitGraphics(struct CollectionMenu *menu)
{
    u32 sequences[30];
    u32 palettes[27];
    struct AnimSpriteResource *resource;
    struct ContactPosition position;
    s32 i;

    memcpy(sequences, gUnk_0824FEB4, sizeof(sequences));
    memcpy(palettes, gUnk_0824FF2C, sizeof(palettes));
    resource = ScriptProc_Start(0xcb05, 0xe2ab);
    menu->resource = *resource;
    ResourceHeader_Relocate(&menu->resource, resource);
    AnimSprite_InitLinked(&menu->background, &menu->resource, 0x56, 0x10, 0, 0, 0, 0);
    AnimSprite_InitLinked(&menu->cursor, &menu->resource, 0x58, 0x10, 0, 0, 0, 0);
    AnimSprite_InitLinked(&menu->completionBadge, &menu->resource, 0x3e, 0x10, 0, 0, 0, 0);
    if (!Collection_IsComplete())
        menu->completionBadge.flags |= 1;
    position.x = 0xb8;
    position.y = 0x68;
    position.z = 0;
    for (i = 0; i < 4; i++) {
        AnimSprite_InitLinked(&menu->countDigits[i], &menu->resource, 0, 0x10, 0, 0, 0, &position);
        position.x += 8;
        if (i == 1)
            position.x += 8;
    }
    AnimSprite_InitLinked(&menu->pageIndicator, &menu->resource, 0x59, 0x10, 0, 0, 0, 0);
    position.x = 0x18;
    position.y = 0x18;
    for (i = 0; i < 27; i++) {
        AnimData_Load(&menu->graphics[i], 0x593a);
        Sprite_Init(&menu->entries[i], &menu->graphics[i], 0x11);
        menu->entries[i].position.x = position.x;
        menu->entries[i].position.y = position.y;
        menu->entries[i].position.z = 0;
        menu->entries[i].priority = 1;
        menu->sequenceResource = ScriptProc_Start(0x922e, 0x4bbf);
        Anim_SetSequence(&menu->sequence, menu->sequenceResource, (u16)sequences[i], 0, 0);
        PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&menu->entries[i], &menu->sequence);
        AnimData_SelectPaletteBank(menu->entries[i].resource, palettes[i] + 0x1f7);
        if (Mod(i, 10) == 4) {
            position.x = 0x18;
            position.y += 0x28;
        } else {
            position.x += 0x28;
        }
        if (Mod(i, 10) == 9) {
            position.x = 0x18;
            position.y = 0x18;
        }
    }
    CpuSet(menu->cursor.palette, menu->cursorColors, 0x10);
    menu->cursor.palette = menu->cursorColors;
    menu->cursor.paletteBank = 0xe010;
}
