// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
static inline s32 randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gRandomTable);
}
static inline u32 getState(struct ScenePanelRig *rig)
{
    u32 value = rig->state;
    return value;
}
static inline s32 randomFirst(void)
{
    u32 offset;
    register const u16 *table asm("r4");
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    table = gRandomTable;
    return *(const u16 *)(offset + (u32)table);
}
void ScenePanelRig_TickResourceDrain(struct ScenePanelRig *rig)
{
    struct SceneRigParentView *parent = (void *)rig->parent;
    s32 count = ScenePanelRig_GetActiveCount(rig), mode = ScenePanelRig_GetParentMode(rig);
    if (count > 0 && mode > 0) {
        s32 rate = 0, drain = 0;
        register u32 stateValue asm("r0") = getState(rig);
        u32 state = stateValue;
        if (state == 2) {
            rate = count * mode * 4;
            if (((randomFirst() >> 3) & 1) == 0) {
                const void *position = &rig->position;
                register u32 effectOffset asm("r4") = 0x6A0;
                asm("" : "+r"(effectOffset) : "r"(position));
                SparkParticle_Spawn(position, 1, (u8 *)rig + effectOffset, 0, 0, 0, 2, 1);
            }
        } else if ((u16)(stateValue - 3) <= 2 || state == 7) {
            rate = count * mode;
            if (((randomValue() >> 3) & 7) == 0) {
                register u32 originOffset asm("r2") = 0x674;
                asm("" : "+r"(originOffset));
                SparkParticle_Spawn((u8 *)rig + originOffset, 1, rig->payloadB, 0, 0, 0, 2, 1);
            }
        }
        if (rate > 0) {
            rate = Div(parent->resourceScale * rate, 100);
            if (rate <= 0)
                rate = 1;
        }
        if ((rig->resourceAccumulator += rate) > 40) {
            drain += Div(rig->resourceAccumulator, 40);
            rig->resourceAccumulator -= drain * 40;
        }
        {
            struct GameState *game = gGameState;
            s32 *remaining;
            register s32 value asm("r0");
            remaining = &game->unk618;
            value = *remaining;
            if (value <= drain)
                value = 0;
            else
                value -= drain;
            *remaining = value;
        }
        {
            struct SceneRigParentView **global = (void *)&gScenePanel;
            struct SceneRigParentView *current = *global;
            u32 offset = 0x1343;
            u8 *warning = (void *)(offset + (u32)current);
            u32 stage = *warning;
            if (stage == 0 && gGameState->unk618 <= gGameState->unk61C >> 2) {
                *warning = 1;
                {
                    struct SceneRigParentView *reload = *global;
                    u32 index = *(u8 *)(offset + (u32)reload);
                    u16 sound = reload->warningSounds[index];
                    if (sound)
                        Sound_PlaySong(sound);
                }
                ScreenTransition_SendCommand(0, 4, 31, 31, 31, 8191, stage);
                ScenePanel_CallPendingScript((void *)*global, 4);
            }
        }
    }
}
