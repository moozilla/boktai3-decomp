#ifndef GUARD_SCENE_PANEL_SUBJECT_H
#define GUARD_SCENE_PANEL_SUBJECT_H
#include "scene_panel_commands.h"
#include "contact_swarm.h"

/* Tentative subject view; state-specific field meanings remain under review. */
struct ScenePanelSubjectView {
    u8 unknown0[0x18];
    struct ContactSwarmSprite drawable;
    u8 graphics[0x1C];
    struct ContactPosition home;
    struct ContactCollider collider;
    struct DisplaySequenceState sequence;
    void *sequenceResource;
    u8 sequenceRow, sequenceVariant, kind, state;
    u16 stateFrame, contactProgress, contactLimit;
    u8 unknownDA[4];
    u16 timer, appearDelay, unknownE2, contactCooldown, requested;
    u16 requestLevel, unknownEA, hold;
    u8 unknownEE, contactKind, changed, positionReset;
    u16 paletteBank, basePaletteBank, countdown, nextThreshold;
    u8 unknownFA[3], byteTimer;
    u8 unknownFE[6];
    u32 effects[0x138 / 4];
    void (*update)(struct ScenePanelSubjectView *);
};
extern void (*gUnk_08605AF8[])(struct ScenePanelSubjectView *);
struct ScenePanelLifecycleView;
s32 ScenePanel_TickLifecycle(struct ScenePanelLifecycleView *);
s32 ScenePanel_DestroyLifecycle(struct ScenePanelLifecycleView *);
s32 ScenePanel_InitializeLifecycle(struct ScenePanelLifecycleView *);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
struct ScenePanel *sub_0805CB38(u32);
void sub_0805CBD4(struct ScenePanelSubjectView *);
void sub_0805CC44(struct ScenePanelSubjectView *);
void sub_0805CCF0(struct ContactCollider *, struct ContactCollider *, void *);
void sub_0805CD60(struct ScenePanelSubjectView *);
#endif
