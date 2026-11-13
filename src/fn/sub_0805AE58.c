#include "scene_panel_rig_setup.h"
#include "monster_particle_random.h"

void ScenePanelRig_InitPrimarySprites(struct ScenePanelRig *rig)
{
    struct ScenePanelRig *owner;
    s32 i;
    s32 compare;
    u32 one;
    {
        struct ScenePanelRig *input = rig;
        owner = input;
    }
    {
        u32 zero = 0;
        i = zero;
    }
    do {
        struct SceneRigSprite *sprite;
        struct SceneRigSprite *animated;
        union {
            void *graphics;
            const u16 *table;
        } work;
        void *resource;
        u16 animationKey;
        sprite = &owner->primary[i];
        animated = sprite;
        work.graphics = sprite->graphics;
        AnimData_Load(work.graphics, (u16)String_Hash(gUnk_0824DEAC));
        {
            u32 parity = i;
            u32 bit;
            bit = 1;
            if (parity & bit) {
                struct SceneRigSprite *arg0 = sprite;
                void *arg1 = work.graphics;
                Sprite_Init(arg0, arg1, 512);
            } else {
                struct SceneRigSprite *arg0 = sprite;
                void *arg1 = work.graphics;
                Sprite_Init(arg0, arg1, 1024);
            }
        }
        {
            u32 zero = 0;
            u32 mode;
            sprite->frame = zero;
            mode = 1;
            sprite->renderMode = mode;
        }
        sprite->position = owner->position;
        {
            u32 phase;
            u32 phaseIndex = i;
            phase = phaseIndex << 5;
            animated->phase = phase;
        }
        work.table = (const u16 *)RANDOM_TABLE_ADDR;
        animated->randomPhase = (u8)ParticleRandom_ReadDirectSample(work.table);
        animationKey = (u16)String_Hash(gUnk_0824DEB0);
        resource = ScriptProc_Start(0x922E, animationKey);
        animated->animationResource = resource;
        Anim_SetSequence(&animated->sequence, animated->animationResource,
                         (u16)(Mod(ParticleRandom_ReadDirectSample(work.table), 3) + 1), 0, 0);
        {
            u32 sample = ParticleRandom_ReadDirectSample(work.table);
            {
                u32 bit = 1;
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
}
