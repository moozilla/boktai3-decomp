#ifndef GUARD_PLAY_STATS_H
#define GUARD_PLAY_STATS_H
#include "scene_input.h"

struct PlayStatsScreen {
    struct Task task;
    struct SceneInputResource resources[2];
    void *layerMap;
    u8 *palettes;
    u32 flags, script, screen, screenTimer;
    s32 state, timer;
    u32 mapId;
    u8 timeText[9], valueText[11];
    struct SceneInputWidget entries[10];
    u32 soundCued;
    void (*callback)(struct PlayStatsScreen *);
    u8 *scriptPc[3];
};
s32 SceneActorManager_Exists(void);
void PlayStats_SetState(struct PlayStatsScreen *, u32);
void PlayStats_RemoveAnimations(struct PlayStatsScreen *);
void PlayStats_HideEntryAnimations(struct PlayStatsScreen *);
void PlayStats_DrawLabels(struct PlayStatsScreen *);
void PlayStats_UpdateResults(struct PlayStatsScreen *);
u32 PlayStats_Update(struct PlayStatsScreen *);

u32 PlayStats_OnTaskDestroy(struct PlayStatsScreen *screen);
void PlayStats_LoadBackground(struct PlayStatsScreen *screen);
void PlayStats_LoadPalette(struct PlayStatsScreen *screen);
void PlayStats_InitializeWidgets(struct PlayStatsScreen *screen);
void PlayStats_LoadResources(struct PlayStatsScreen *screen);
s32 PlayStats_Initialize(struct PlayStatsScreen *screen);
struct PlayStatsScreen *PlayStats_Create(void);
#endif
