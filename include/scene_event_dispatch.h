#ifndef GUARD_SCENE_EVENT_DISPATCH_H
#define GUARD_SCENE_EVENT_DISPATCH_H
#include "global.h"
struct SceneEventCommand {
    u8 unknown0[6];
    u8 kind, unknown7;
    s16 value;
};
struct SceneEventReceiver {
    u8 unknown0[6];
    u8 count[2];
    struct SceneEventCommand *pending[2][4];
};
struct SceneEventDispatchTask {
    u8 task[0x1C];
    struct SceneEventReceiver receiver;
};
struct SceneEventDispatchState {
    u8 unknown0[0x24];
    u8 phase;
};
extern struct SceneEventDispatchState *gUnk_0200047C;
void sub_08020D54(struct SceneEventReceiver *, struct SceneEventCommand *);
void sub_08063634(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                  struct SceneEventCommand *);
void sub_08063650(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                  struct SceneEventCommand *);
void sub_0806366C(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                  struct SceneEventCommand *);
void sub_08063688(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                  struct SceneEventCommand *);
void sub_080636A4(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                  struct SceneEventCommand *);
void sub_080636C0(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                  struct SceneEventCommand *);
void sub_080636DC(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                  struct SceneEventCommand *);
void sub_080636F8(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                  struct SceneEventCommand *);
void sub_08063714(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                  struct SceneEventCommand *);
void sub_08063730(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                  struct SceneEventCommand *);
#endif
