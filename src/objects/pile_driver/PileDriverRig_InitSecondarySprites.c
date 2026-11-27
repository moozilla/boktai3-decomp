#include "objects/pile_driver/pile_driver_rig_setup.h"

static inline u16 PileDriverRig_NextRandom(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)gRandomTable);
}

void PileDriverRig_InitSecondarySprites(struct PileDriverRig *rig)
{
    s32 i;
    for (i = 0; i < 6; i++) {
        struct SceneRigSprite *sprite = &rig->secondary[i];
        struct SceneRigSprite *animated = sprite;
        AnimData_Load(sprite->graphics, (u16)String_Hash(gUnk_0824DEAC));
        if (i & 1)
            Sprite_Init(sprite, sprite->graphics, 512);
        else
            Sprite_Init(sprite, sprite->graphics, 1024);
        sprite->frame = 0;
        if (i == 0)
            sprite->renderMode = 2;
        else
            sprite->renderMode = 1;
        sprite->position = rig->position;
        animated->phase = i << 5;
        animated->randomPhase = (u8)PileDriverRig_NextRandom();
        animated->animationResource = ScriptProc_Start(0x922E, (u16)String_Hash(gUnk_0824DEB0));
        Anim_SetSequence(&animated->sequence, animated->animationResource, (u16)(5 - ((i + 1) >> 1)), 0, 0);
        if (i & 1)
            sprite->flags |= 4;
    }
}
