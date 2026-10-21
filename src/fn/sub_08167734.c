#include "global.h"
#include "engine.h"

struct PanelParticle {
    u8 unknown[0x40];
};
struct PanelParticleGroup {
    struct PanelParticle particles[8];
    u8 unknown200[8];
};
struct PanelEffect {
    u8 unknown[0x28];
};
struct PlayerPanel {
    u8 unknown0[0x4458];
    struct PanelParticleGroup groups[2];
    u8 unknown4868[0x478];
    struct PanelEffect effects[4];
};
void PlayerPanel_RemoveEffects(struct PlayerPanel *panel)
{
    s32 group = 0;
    u32 offset = 0;
    s32 i;
    do {
        struct PanelParticle *particle = (struct PanelParticle *)((u8 *)panel + (offset + 0x4458));
        for (i = 7; i >= 0; i--) {
            Effect_Remove(particle);
            particle++;
        }
        offset += 0x208;
        (*(&group))++;
    } while (group < 2);
    {
        struct PanelEffect *effect = panel->effects;
        for (group = 3; group >= 0; group--) {
            Effect_Remove(effect);
            effect++;
        }
    }
}
