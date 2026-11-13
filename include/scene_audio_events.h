#ifndef GUARD_SCENE_AUDIO_EVENTS_H
#define GUARD_SCENE_AUDIO_EVENTS_H

#include "scene_messages.h"

struct SceneEventDispatchTask {
    struct Task task; /* 0x00 */
    u32 unknown18;
    struct SceneMessageReceiver messages; /* 0x1C */
};

s32 SceneAudioEvent_DispatchPending(struct SceneEventDispatchTask *);
s32 SceneAudioEvent_Tick(struct SceneEventDispatchTask *);
s32 SceneAudioEvent_Destroy(void);
s32 SceneAudioEvent_Register(struct SceneEventDispatchTask *, s32 receiverId);
struct SceneEventDispatchTask *SceneAudioEvent_Create(s32 receiverId);

void Sound_StartSongDirect(s32 song);
void Sound_PlayTrackedSong(s32 song);
void Sound_FadeInChannel10(s32 duration);
void Sound_PauseChannel10(s32 duration);
void Sound_FadeOutChannel10(s32 duration);
void Sound_PlayChannel12Song(s32 song);
void Sound_FadeInChannel12(s32 duration);
void Sound_PauseChannel12(s32 duration);
void Sound_FadeOutChannel12(s32 duration);

#endif
