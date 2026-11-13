#include "scene_panel_rig_setup.h"

void ScenePanelRig_InitSecondarySprites(struct ScenePanelRig *rig)
{
    struct ScenePanelRig *owner = rig;
    s32 i = 0;
    u32 one = 1;
    u32 zero;
    zero = i;
    do {
        struct SceneRigSprite *sprite;
        struct SceneRigSprite *animated;
        s32 next;
        void *graphics, *resource;
        sprite = &owner->secondary[i];
        animated = sprite;
        graphics = sprite->graphics;
        AnimData_Load(graphics, (u16)String_Hash(gUnk_0824DEAC));
        {
            u32 parity = i & 255;
            {
                u32 bit = one;
                parity &= bit;
            }
            if (parity)
                Sprite_Init(sprite, graphics, 512);
            else
                Sprite_Init(sprite, graphics, 1024);
        }
        {
            u32 value = zero;
            sprite->frame = value;
        }
        {
            u32 mode;
            if (i == 0) {
                mode = 2;
                sprite->renderMode = mode;
            } else {
                mode = one;
                sprite->renderMode = mode;
            }
        }
        sprite->position = owner->position;
        animated->phase = i << 5;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 offset = gRandomIndex * 2;
            const u8 *table;
            table = (void *)RANDOM_TABLE_ADDR;
            offset += (u32)table;
            animated->randomPhase = *(u8 *)offset;
        }
        resource = ScriptProc_Start(0x922E, (u16)String_Hash(gUnk_0824DEB0));
        animated->animationResource = resource;
        Anim_SetSequence(&animated->sequence, resource, (u16)({
            next = i + 1;
            5 - (next >> 1);
        }),
                         0, zero);
        if (i & one)
            sprite->flags |= 4;
        i = next;
    } while (i <= 5);
}
