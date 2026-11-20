#include "player_attack_particles.h"
#include "engine.h"

#define PlayerAttackEffect_Initialize sub_081625C0
s32 PlayerAttackEffect_Initialize(struct AttackParticleEffect *effect,
                                  struct AttackParticleOwner *source, u8 *facing, u8 *side)
{
    struct SceneAnimation *graphics = effect->graphics;
    struct DisplaySequenceSprite *entry;
    s32 i;

    AnimData_Load(graphics, 0x7E5F);
    graphics++;
    AnimData_Load(graphics, 0x6250);
    AnimData_SelectPaletteBank(graphics, 0x116);
    graphics++;
    AnimData_Load(graphics, 0x6250);
    AnimData_SelectPaletteBank(graphics, 0x118);
    graphics++;
    AnimData_Load(graphics, 0x6250);
    AnimData_SelectPaletteBank(graphics, 0x119);
    graphics++;
    AnimData_Load(graphics, 0x6250);
    AnimData_SelectPaletteBank(graphics, 0x11A);
    graphics++;
    AnimData_Load(graphics, 0x6250);
    AnimData_SelectPaletteBank(graphics, 0x11B);
    entry = effect->sprites;
    for (i = 0; i < 16; i++) {
        Sprite_Init(entry, graphics, 1);
        entry->frame = 0;
        entry->position = source->position;
        entry->priority = 1;
        effect->slots[i].life = 0;
        entry++;
    }
    effect->sequenceResource = ScriptProc_Start(0x922E, 0x981D);
    effect->owner = source;
    effect->facing = facing;
    effect->side = side;
    effect->frame = 0;
    return 0;
}