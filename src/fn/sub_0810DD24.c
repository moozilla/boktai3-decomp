#include "global.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
struct PreviewPosition {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
struct PreviewSource {
    u8 unk0[0x53];
    u8 active;
    u8 unk54[0x14c];
    u8 values[5];
};
extern struct PreviewSource *gUnk_020004B4;
extern u32 gUnk_030053F0;
void CharacterModel_InitPalettes(u8 *, s32);
void ActorSprite_InitPartDrawings(void *, void *, u32);
void CharacterPreview_PositionParts(void *, void *, void *, s32);
void CharacterModel_SetAnimationFlags(u8 *, u32);
void CharacterModel_SetComponentRenderMode(void *, s32);
void SceneActorStyle_InitializeFromGameState(void *);
void SelectionPanel_InitPreview(u8 *p)
{
    struct PreviewPosition pos;
    u8 *base, *sprite;
    u32 frame;
    u32 mask = 0x800;
    u32 flags = (gEngineFlags | gUnk_030053F0) & mask;
    if (flags) {
        register u32 offset asm("r4");
        struct PreviewSource *source = gUnk_020004B4;
        u8 *values;
        u8 value;
        u8 zero;
        u8 *dest;
        if (!source || source->active != 1)
            return;
        values = source->values;
        if (!values)
            return;
        base = p + 0x494;
        value = values[0];
        dest = p + 0x4dc;
        zero = 0;
        *dest = value;
        value = values[2];
        offset = 0x4de;
        p[offset] = value;
        value = values[1];
        offset--;
        p[offset] = value;
        value = values[4];
        offset += 3;
        p[offset] = value;
        value = values[3];
        offset--;
        p[offset] = value;
        asm("" ::"r"(offset));
        frame = values[3];
        p[0xcde] = zero;
    } else {
        base = p + 0x494;
        SceneActorStyle_InitializeFromGameState(p + 0x4dc);
        frame = p[0x4df];
        p[0xcde] = flags;
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
