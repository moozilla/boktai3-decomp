#ifndef GUARD_SCENE_PANEL_SUBJECT_H
#define GUARD_SCENE_PANEL_SUBJECT_H
#include "scene_panel_commands.h"

/* Tentative subject view; state-specific field meanings remain under review. */
struct ScenePanelSubjectView {
    u8 unknown0[0x34];
    struct ContactPosition position;
    u8 unknown3C[0x24];
    struct ContactPosition home;
    u8 unknown68[0x6B];
    u8 state;
    u16 stateFrame, contactProgress, contactLimit;
    u8 unknownDA[0xA];
    u16 contactCooldown;
    u8 unknownE6[9];
    u8 contactKind, changed, positionReset;
    u8 unknownF2[0x14A];
    void (*update)(struct ScenePanelSubjectView *);
};
extern void (*gUnk_08605AF8[])(struct ScenePanelSubjectView *);
struct ScenePanelLifecycleView;
s32 sub_0805C8D8(struct ScenePanelLifecycleView *);
s32 sub_0805CA20(struct ScenePanelLifecycleView *);
s32 sub_0805CA7C(struct ScenePanelLifecycleView *);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
#endif
