#include "scene_panel_rig_setup.h"

// MATCHING_CONCESSION: register-pinning

#define ScenePanelRig_InitSecondarySprites sub_0805AF88
void ScenePanelRig_InitSecondarySprites(struct ScenePanelRig *rig)
{
    struct ScenePanelRig *owner = rig;
    s32 i = 0;
    register u32 one asm("r9") = 1;
    register u32 zero asm("r10") = i;
    asm("" : "+r"(owner), "+r"(i), "+r"(one), "+r"(zero));
    do {
        register struct SceneRigSetupSprite *sprite asm("r5");
        struct SceneRigSetupSprite *animated;
        s32 next;
        void *graphics, *resource;
        {
            register u32 offset asm("r0") = 0x68 * i;
            register struct ScenePanelRig *base asm("r3");
            offset += 0x3A8;
            base = owner;
            sprite = (void *)((u8 *)base + offset);
        }
        animated = sprite;
        graphics = sprite->graphics;
        AnimData_Load(graphics, (u16)sub_08219BD8(gUnk_0824DEAC));
        {
            u32 parity = i;
            asm("" : "+r"(parity));
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
            register u32 value asm("r3") = zero;
            sprite->scale = value;
        }
        {
            u32 mode;
            if (i == 0) {
                mode = 2;
                asm("" : "+r"(mode));
            } else {
                mode = one;
            }
            sprite->renderMode = mode;
        }
        sprite->position = owner->position;
        animated->phase = i << 5;
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 offset = gUnk_03005308 * 2;
            const u8 *table;
            table = (void *)RANDOM_TABLE_ADDR;
            asm("" : "+r"(table));
            offset += (u32)table;
            animated->randomPhase = *(u8 *)offset;
        }
        resource = ScriptProc_Start(0x922E, (u16)sub_08219BD8(gUnk_0824DEB0));
        animated->animationResource = resource;
        Anim_SetSequence(animated->animation, resource, (u16)({
                             next = i + 1;
                             5 - (next >> 1);
                         }),
                         0, zero);
        if (i & one)
            sprite->flags |= 4;
        i = next;
    } while (i <= 5);
}
