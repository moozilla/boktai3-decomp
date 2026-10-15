#ifndef GUARD_SCENE_PANEL_SUBJECT_TAIL_H
#define GUARD_SCENE_PANEL_SUBJECT_TAIL_H
#include "scene_panel_subject.h"

/* A separate view until the subject state-handler layouts are integrated. */
struct ScenePanelSubjectDrawable {
    u32 flags;
    u8 unknown4[0x18];
    struct ContactPosition position;
    u8 unknown24[8];
};
struct ScenePanelSubjectTail {
    u8 unknown0[0x18];
    struct ScenePanelSubjectDrawable drawable;
    u8 graphics[0x1C];
    struct ContactPosition home;
    struct ContactCollider collider;
    struct DisplaySequenceState sequence;
    void *resource;
    u8 row, variant, kind, state;
    u16 stateFrame, contactProgress, contactLimit;
    u8 unknownDA[4];
    u16 timer, appearDelay, unknownE2, contactCooldown, requested;
    u16 requestLevel, unknownEA, hold;
    u8 unknownEE, contactKind, changed, positionReset;
    u16 tile, baseTile, countdown, nextThreshold;
    u8 unknownFA[3], byteTimer;
    u8 unknownFE[6];
    u32 effects[0x138 / 4];
    void (*update)(struct ScenePanelSubjectTail *);
};
void sub_08231568(void *);
void sub_082314C4(void *);
u32 sub_0823233C(void *);
void sub_0821FF7C(void *, u32, u32, u32);
void sub_0805D6C4(struct ScenePanelSubjectTail *);
void sub_0805D770(struct ScenePanelSubjectTail *);
s32 sub_0805D800(struct ScenePanelSubjectTail *);
s32 sub_0805D884(struct ScenePanelSubjectTail *);
void sub_0805D8A8(struct ScenePanelSubjectTail *, u32);
#endif
