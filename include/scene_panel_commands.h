#ifndef GUARD_SCENE_PANEL_COMMANDS_H
#define GUARD_SCENE_PANEL_COMMANDS_H
#include "scene_panel_orchestration.h"

struct ScenePanelCommand {
    u8 unknown0[6];
    u8 opcode, unknown7;
    u16 mode;
};
struct ScenePanelReceiver {
    u16 actorId;
    u8 category, pendingTrigger;
    u8 unknown4[2], count[2];
    struct ScenePanelCommand *commands[2][4];
};
struct ScenePanelCommandView {
    u8 unknown0[0x18];
    u16 actorId;
    u8 unknown1A[0x17B0 - 0x1A];
    struct ScenePanelReceiver receiver;
};
struct ScenePanelCommandGlobals {
    u8 unknown0[0x24];
    u8 bank;
};
extern struct ScenePanelCommandGlobals *gUnk_0200047C;
u32 sub_08020D54(void *, void *);
void sub_08020D08(void *);
s32 SceneMessages_RegisterReceiver(void *, u32, u32);
void sub_08225F88(const void *);

void sub_0805C680(struct ScenePanel *);
void sub_0805C728(struct ScenePanelCommandView *);
void sub_0805C73C(struct ScenePanelCommandView *);
void sub_0805C754(void);
u32 sub_0805C760(void);
void sub_0805C774(struct ScenePanelInteractionView *);
s32 sub_0805C7B4(struct ScenePanelInteractionView *);
void sub_0805C7E4(struct ScenePanelInteractionView *);
void sub_0805C86C(struct ScenePanelInteractionView *);
#endif
