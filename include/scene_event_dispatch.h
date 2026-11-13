#ifndef GUARD_SCENE_EVENT_DISPATCH_H
#define GUARD_SCENE_EVENT_DISPATCH_H
#include "global.h"
struct SceneEventCommand {
    u16 id;
    u8 receiverKind, trigger;
    u16 unknown4;
    u8 kind, argumentCount;
    s16 value;
};
struct SceneEventReceiver {
    u16 id;
    u8 kind, trigger;
    u16 unknown4;
    u8 count[2];
    struct SceneEventCommand *pending[2][4];
};
struct SceneEventDispatchTask;
struct SceneEventDispatchState {
    u8 unknown0[0x24];
    u8 phase;
};
/* 0x0200047C */
extern struct SceneEventDispatchState *gSceneEventDispatchState;
u32 SceneMessages_SetPendingTrigger(struct SceneEventReceiver *, struct SceneEventCommand *);
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
