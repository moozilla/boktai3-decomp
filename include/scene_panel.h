#ifndef GUARD_SCENE_PANEL_H
#define GUARD_SCENE_PANEL_H
#include "engine.h"
#include "background_controller.h"
/* Tentative presentation/state view; fields belong to observed consumers. */
struct ScenePanel {
    u8 unknown0[0x22E];
    u16 stateFrame, channels[8];
    u8 unknown240[0x10];
    void (*state)(struct ScenePanel *);
    u8 unknown254[0x280];
    void *backgroundMap, *palette;
    u8 unknown4DC[0x734];
    u8 paletteMode;
    u8 unknownC11[0x717];
    u16 mode;
    u8 unknown132A[0x10];
    u8 resourceIndex;
    u8 unknown133B[0xED];
    u32 transitionComplete;
    u8 unknown142C[4];
    u16 channelsDirty;
};
extern const u16 gUnk_08605A88[];
extern u16 gUnk_03004BD8;
void sub_08058130(struct ScenePanel *, s32, u16);
void sub_08058150(struct ScenePanel *, void (*)(struct ScenePanel *));
s32 sub_08058544(struct ScenePanel *);
void sub_08058168(struct ScenePanel *);
void sub_08058184(struct ScenePanel *);
void sub_080581A0(struct ScenePanel *);
void sub_0805822C(struct ScenePanel *);
void sub_080582B8(struct ScenePanel *);
void sub_08058444(struct ScenePanel *);
void sub_080585AC(struct ScenePanel *);
void sub_080586B0(struct ScenePanel *);
void sub_080581C0(struct ScenePanel *);
void sub_080581F8(struct ScenePanel *);
void sub_0805824C(struct ScenePanel *);
void sub_08058284(struct ScenePanel *);
void sub_080582EC(struct ScenePanel *);
void sub_08058324(struct ScenePanel *);
void sub_0805835C(struct ScenePanel *);
void sub_08058394(struct ScenePanel *);
void sub_080583CC(struct ScenePanel *);
void sub_08058404(struct ScenePanel *);
void sub_08058464(struct ScenePanel *);
void sub_0805849C(struct ScenePanel *);
void sub_080584D4(struct ScenePanel *);
void sub_0805850C(struct ScenePanel *);
#endif
