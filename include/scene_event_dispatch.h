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
/* 0x0200047C */
extern struct SceneEventDispatchState *gSceneEventDispatchState;
void SceneMessages_SetPendingTrigger(struct SceneEventReceiver *, struct SceneEventCommand *);
void SceneAudioEvent_PlaySong(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                              struct SceneEventCommand *);
void SceneAudioEvent_StartSongDirect(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                                     struct SceneEventCommand *);
void SceneAudioEvent_StartTrackedSong(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                                      struct SceneEventCommand *);
void SceneAudioEvent_FadeInChannel10(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                                     struct SceneEventCommand *);
void SceneAudioEvent_PauseChannel10(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                                    struct SceneEventCommand *);
void SceneAudioEvent_FadeOutChannel10(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                                      struct SceneEventCommand *);
void SceneAudioEvent_StartChannel12(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                                    struct SceneEventCommand *);
void SceneAudioEvent_FadeInChannel12(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                                     struct SceneEventCommand *);
void SceneAudioEvent_PauseChannel12(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                                    struct SceneEventCommand *);
void SceneAudioEvent_FadeOutChannel12(struct SceneEventDispatchTask *, struct SceneEventReceiver *,
                                      struct SceneEventCommand *);
#endif
