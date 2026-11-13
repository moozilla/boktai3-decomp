#ifndef GUARD_SCENE_MESSAGES_H
#define GUARD_SCENE_MESSAGES_H

#include "engine.h"
#include "scene_event_dispatch.h"

struct SceneMessageReceiver {
    struct SceneEventReceiver queue; /* 0x00: shared with embedded event receivers */
    u32 unknown28;
    struct SceneMessageReceiver *prev, *next; /* 0x2C */
};

struct SceneMessages {
    struct Task task;                       /* 0x00 */
    s16 scene, step;                        /* 0x18 */
    u16 queued;                             /* 0x1C */
    u8 advance, changed;                    /* 0x1E */
    u16 onFinish;                           /* 0x20 */
    u8 scriptCount, active;                 /* 0x22 */
    u8 buffer, unused25[3];                 /* 0x24 */
    u16 ids[16], scripts[16];               /* 0x28 */
    s32 delay, unknown6C, signal;           /* 0x68 */
    struct SceneMessageReceiver *receivers; /* 0x74 */
    struct SceneMessageReceiver self;       /* 0x78 */
};

struct SceneMessageArguments {
    u32 header; /* low 16 bits: argument count; high 16 bits: preserved */
    s32 *values;
};

struct SceneMessageReceiver *SceneMessages_FindReceiver(struct SceneMessages *, u32 id, u32 kind);
u32 SceneMessages_LinkReceiver(struct SceneMessages *, struct SceneMessageReceiver *);
u32 SceneMessages_UnlinkReceiver(struct SceneMessages *, struct SceneMessageReceiver *);
s32 SceneMessages_Enqueue(struct SceneMessages *, struct SceneEventCommand *);
void SceneMessages_SwapBuffers(struct SceneMessages *);
void SceneMessages_RunScript(struct SceneMessages *, struct SceneEventReceiver *,
                             struct SceneEventCommand *);
void SceneMessages_SetDelay(struct SceneMessages *, struct SceneEventReceiver *,
                            struct SceneEventCommand *);
void SceneMessages_SetSignal(struct SceneMessages *, struct SceneEventReceiver *,
                             struct SceneEventCommand *);
s32 SceneMessages_Dispatch(struct SceneMessages *);
s32 SceneMessages_Update(struct SceneMessages *);
u32 SceneMessages_Destroy(struct SceneMessages *);
s32 SceneMessages_Initialize(struct SceneMessages *, s32 id);
struct SceneMessages *SceneMessages_Create(s32 id);
s32 SceneMessages_RegisterReceiver(struct SceneMessageReceiver *, s32 id, s32 kind);
s32 SceneMessages_UnregisterReceiver(struct SceneMessageReceiver *);
s32 SceneMessages_RequestAdvance(void);
s32 SceneMessages_FindScript(s32 id);
s32 SceneMessages_Send(struct SceneEventCommand *);
s32 SceneMessages_ScriptStart(void);
s32 SceneMessages_Stop(void);
s32 SceneMessages_ConsumeSignal(void);
s32 SceneMessages_IsActive(void);
struct SceneEventCommand *SceneMessages_GetEventMessage(s32 scene, s32 step, u32 index);

#endif
