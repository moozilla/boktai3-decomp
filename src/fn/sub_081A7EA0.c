#define ScenePanel_RequestPairReaction sub_081A7EA0
#include "global.h"
struct ScenePanelPairReaction {
    u8 unknown0[0x9C];
    u32 flags;
    u8 unknownA0[0xAC - 0xA0];
    u8 mode, substate, transition, unknownAF, pending;
    u8 unknownB1[0xC4 - 0xB1];
    u32 counter;
    u8 unknownC8[0x56C - 0xC8];
    void *pair;
    u8 unknown570[0x5C2 - 0x570];
    s16 pairMode;
};
s32 ScenePair_CountPositiveCounters(void *);
u32 Vanargand_IsEitherComponentFlag40Set(void *);
static inline void ScenePanel_RequestReaction(struct ScenePanelPairReaction *panel, u32 mode,
                                              u32 substate, u32 transition)
{
    panel->mode = mode;
    panel->substate = substate;
    panel->transition = transition;
    panel->counter = 0;
    panel->pending = 1;
}
s32 sub_081A7EA0(struct ScenePanelPairReaction *panel)
{
    u32 two = 2;
    if (panel->flags & two) {
        ScenePanel_RequestReaction(panel, 1, 0, 6);
        return 1;
    }
    if (panel->flags & 0x10000) {
        ScenePanel_RequestReaction(panel, 1, 0, 3);
        return 1;
    }
    if (panel->flags & 1) {
        if (ScenePair_CountPositiveCounters(panel->pair) == 0)
            ScenePanel_RequestReaction(panel, 1, 0, 1);
        else
            ScenePanel_RequestReaction(panel, 1, 0, 0);
        return 1;
    }
    if (Vanargand_IsEitherComponentFlag40Set(panel->pair)) {
        ScenePanel_RequestReaction(panel, 0, 0, two);
        return 1;
    }
    if (ScenePair_CountPositiveCounters(panel->pair) == 0 && panel->pairMode != 1 &&
        panel->transition != 11 && panel->transition != 14 && panel->transition != 1) {
        ScenePanel_RequestReaction(panel, 0, 0, 11);
        return 1;
    }
    return 0;
}
