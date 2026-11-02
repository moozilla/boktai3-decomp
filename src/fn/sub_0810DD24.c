#include "global.h"
#include "engine.h"
#include "ram.h"
#include "area_effect_link.h"
struct PreviewPosition {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
struct PreviewSource {
    u8 unk0[0x53];
    u8 active;
    u8 unk54[0x14c];
    u8 values[5];
};
void CharacterModel_InitPalettes(u8 *, s32);
void ActorSprite_InitPartDrawings(void *, void *, u32);
void CharacterPreview_PositionParts(void *, void *, void *, s32);
void CharacterModel_SetAnimationFlags(u8 *, u32);
void CharacterModel_SetComponentRenderMode(void *, s32);
void SceneActorStyle_InitializeFromGameState(void *);
struct SelectionPreview {
    u8 beforeParts[0x494];
    u8 parts[0x48];
    u8 kind, variant, form, palette, extra;
    u8 afterStyle[0xCDE - 0x4E1];
    u8 owner;
};

void SelectionPanel_InitPreview(struct SelectionPreview *menu)
{
    struct PreviewPosition pos;
    u8 *base, *sprite;
    u32 frame;
    u32 mask = 0x800;
    u32 flags = (gEngineFlags | gScenePauseFlags) & mask;
    if (flags) {
        struct PreviewSource *source = gLinkSession;
        u8 *values;
        if (!source || source->active != 1)
            return;
        values = source->values;
        if (!values)
            return;
        base = menu->parts;
        menu->kind = values[0];
        menu->form = values[2];
        menu->variant = values[1];
        menu->extra = values[4];
        menu->palette = values[3];
        frame = values[3];
        menu->owner = 0;
    } else {
        base = menu->parts;
        SceneActorStyle_InitializeFromGameState(&menu->kind);
        frame = menu->palette;
        menu->owner = flags;
    }
    pos.x = 0x30;
    pos.y = 0x50;
    pos.z = 0;
    sprite = base + 0x5f8;
    ActorSprite_InitPartDrawings(base, sprite, 0x3d99);
    CharacterPreview_PositionParts(base, sprite, &pos, 0);
    CharacterModel_SetAnimationFlags(sprite, 0);
    CharacterModel_SetComponentRenderMode(sprite, 0);
    CharacterModel_InitPalettes(sprite, frame);
}
