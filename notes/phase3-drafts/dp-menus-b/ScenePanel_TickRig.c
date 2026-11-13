#include "scene_panel_rig.h"
struct SceneRigDriverView {
    u8 unknown0[0xC0F];
    u8 suppressDrain;
    u8 unknownC10[0x5C];
    struct ScenePanelRig rig;
    u8 unknown1320[0x2C];
    volatile u16 frame, interval;
    u8 unknown1350[0x10];
    void *action;
};
extern void *gUnk_08605A94[];
void ScenePanelRig_CheckContactTransition(struct ScenePanelRig *);
void ScenePanelRig_TickResourceDrain(struct ScenePanelRig *);
void ScenePanelRig_TickSprites(struct ScenePanelRig *, u32);
void ScenePanel_TickRig(struct ScenePanel *input)
{
    struct SceneRigDriverView *parent = (void *)input;
    struct ScenePanelRig *rig = &parent->rig;
    u32 palette;
    ScenePanelRig_CheckContactTransition(rig);
    Callback_InvokeActor(rig, gUnk_08605A94[parent->rig.state]);
    if (parent->rig.colliderHold) {
        u16 *flags = &parent->rig.collider.flags;
        u32 value = *flags;
        u32 mask = 4;
        mask |= value;
        *flags = mask;
        parent->rig.colliderHold--;
    } else {
        u16 *flags = &parent->rig.collider.flags;
        u32 value = *flags;
        u32 mask = 0xFFFB;
        mask &= value;
        *flags = mask;
        Collider_SetPosition(&parent->rig.collider, &parent->rig.position, 0);
    }
    if (!parent->suppressDrain)
        ScenePanelRig_TickResourceDrain(rig);
    palette = 91;
    if (parent->action) {
        volatile u16 *framePtr = &parent->frame;
        volatile u16 *intervalPtr = &parent->interval;
        u32 first = *framePtr;
        u32 interval = *intervalPtr;
        if (first >= interval)
            palette = 92;
        else {
            u32 frame = *framePtr;
            frame &= 0xFFFF;
            if ((s32)frame >= (s32)*intervalPtr - 90 && ((frame >> 2) & 1))
                palette = 92;
        }
    }
    ScenePanelRig_TickSprites(rig, palette);
}
