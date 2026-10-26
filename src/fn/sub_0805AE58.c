#include "scene_panel_rig_setup.h"

// MATCHING_CONCESSION: register-pinning

void ScenePanelRig_InitPrimarySprites(struct ScenePanelRig *rig)
{
    struct ScenePanelRig *owner;
    register s32 i asm("r8");
    s32 compare;
    u32 indexMask;
    register u32 one asm("r0");
    u32 *randomIndex = &gRandomIndex;
    {
        struct ScenePanelRig *input = rig;
        owner = input;
    }
    {
        register u32 zero asm("r0") = 0;
        i = zero;
    }
    asm("" : "+r"(randomIndex));
    {
        register u32 initialMask asm("r1") = 1023;
        asm("" : "+r"(initialMask));
        indexMask = initialMask;
    }
    /* Keep the copies distinct without enlarging the short loop branch estimate. */
    asm("" : : "g"(&&primary_offset_ready), "g"(&&primary_parity_ready));
    do {
        register struct SceneRigSprite *sprite asm("r5");
        register struct SceneRigSprite *animated asm("r6");
        const u16 *table;
        void *graphics, *resource;
        u16 animationKey;
        {
            register u32 offset asm("r0") = 0x68;
            register u32 product asm("r3");
            struct ScenePanelRig *base;
            product = i;
            product *= offset;
        primary_offset_ready:
            offset = product;
            offset += 0x68;
            base = owner;
            sprite = (void *)((u8 *)base + offset);
        }
        animated = sprite;
        graphics = sprite->graphics;
        AnimData_Load(graphics, (u16)String_Hash(gUnk_0824DEAC));
        {
            register u32 parity asm("r0") = i;
            register u32 bit asm("r3");
        primary_parity_ready:
            bit = 1;
            if (parity & bit) {
                struct SceneRigSprite *arg0 = sprite;
                register void *arg1 asm("r1") = graphics;
                Sprite_Init(arg0, arg1, 512);
            } else {
                struct SceneRigSprite *arg0 = sprite;
                register void *arg1 asm("r1") = graphics;
                Sprite_Init(arg0, arg1, 1024);
            }
        }
        {
            register u32 zero asm("r0") = 0;
            register u32 mode asm("r1");
            sprite->frame = zero;
            mode = 1;
            sprite->renderMode = mode;
        }
        sprite->position = owner->position;
        {
            register u32 phase asm("r0");
            u32 phaseIndex = i;
            phase = phaseIndex << 5;
            animated->phase = phase;
        }
        table = (const u16 *)RANDOM_TABLE_ADDR;
        {
            u32 next = (*randomIndex) + 1;
            u32 mask = indexMask;
            (*randomIndex) = next & mask;
        }
        animated->randomPhase = *(u8 *)&table[(*randomIndex)];
        animationKey = (u16)String_Hash(gUnk_0824DEB0);
        resource = ScriptProc_Start(0x922E, animationKey);
        animated->animationResource = resource;
        {
            u32 next = (*randomIndex) + 1;
            u32 mask = indexMask;
            (*randomIndex) = next & mask;
        }
        Anim_SetSequence(&animated->sequence, animated->animationResource,
                         (u16)(Mod(table[(*randomIndex)], 3) + 1), 0, 0);
        {
            u32 next = (*randomIndex) + 1;
            u32 mask = indexMask;
            (*randomIndex) = next & mask;
        }
        {
            u32 sample = table[(*randomIndex)];
            {
                register u32 bit asm("r3") = 1;
                sample &= bit;
                if (sample)
                    sprite->flags |= 4;
            }
        }
        {
            one = 1;
            i += one;
            compare = i;
        }
    } while (compare <= 7);
    asm("" : : "r"(one), "r"(compare));
}
