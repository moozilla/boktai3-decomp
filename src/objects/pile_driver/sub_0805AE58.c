#include "objects/pile_driver/scene_panel_rig_setup.h"
#include "engine/math/monster_particle_random.h"

void PileDriverRig_InitPrimarySprites(struct ScenePanelRig *rig)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        struct SceneRigSprite *sprite = &rig->primary[i];
        struct SceneRigSprite *animated = sprite;
        union {
            void *graphics;
            const u16 *table;
        } work;
        void *resource;
        u16 animationKey;
        work.graphics = sprite->graphics;
        AnimData_Load(work.graphics, (u16)String_Hash(gUnk_0824DEAC));
        {
            u32 parity = i;
            if (parity & 1) {
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
            sprite->frame = 0;
            sprite->renderMode = 1;
        }
        sprite->position = rig->position;
        {
            u32 phase;
            u32 phaseIndex = i;
            phase = phaseIndex << 5;
            animated->phase = phase;
        }
        work.table = gRandomTable;
        animated->randomPhase = (u8)ParticleRandom_ReadDirectSample(work.table);
        animationKey = (u16)String_Hash(gUnk_0824DEB0);
        resource = ScriptProc_Start(0x922E, animationKey);
        animated->animationResource = resource;
        Anim_SetSequence(&animated->sequence, animated->animationResource,
                         (u16)(Mod(ParticleRandom_ReadDirectSample(work.table), 3) + 1), 0, 0);
        {
            u32 sample = ParticleRandom_ReadDirectSample(work.table);
            {
                sample &= 1;
                if (sample)
                    sprite->flags |= 4;
            }
        }
    }
}
