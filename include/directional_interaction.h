#ifndef DIRECTIONAL_INTERACTION_H
#define DIRECTIONAL_INTERACTION_H
#include "global.h"
#include "game_state.h"
#include "player.h"

/*
 * Directional interaction controllers (tentative): invisible trigger zones kept in a small pool
 * owned by a task (struct DirectionalManager). A controller watches the player actor: when the
 * actor is close enough and faces the controller from the right side, the controller leaves its
 * idle state (0), and when the action button is pressed it becomes the manager's selected
 * controller and runs its start/end scripts. The "level" counts a short fade of its marker.
 */

/* Marker draw record (embedded in a controller at +0x24). */
struct DirectionalDrawable {
    u32 flags; /* bit 0: hidden, bit 2: oriented along z */
    u8 pad4[3];
    u8 mode;
    u8 pad8[8];
    u16 scale;
    u8 pad12[0x1c - 0x12];
    struct SceneVec position; /* marker position, derived from the controller's */
    u8 pad24[0x2c - 0x24];
};

struct DirectionalController {
    u16 id;
    u8 orientation; /* 0: faces -x/-z side, else +x side */
    u8 flags;
    u8 disabled;
    u8 state; /* 0 idle, 1 approached, 2 selected */
    u8 entered;
    u8 level; /* marker fade step, 0-5 */
    s32 region;
    u16 startScript, endScript;
    u32 distance; /* squared trigger distance */
    s16 arguments[4];
    struct SceneVec position;
    struct DirectionalDrawable drawable;
    u8 animation[28];
};

struct DirectionalManager {
    u8 task[24];
    u32 count : 8;
    s32 selected : 8; /* controller whose script is running, or -1 */
    s32 pending : 8;  /* 0: run start script, 1: run end script, -1: none */
    u32 unused : 8;
    u32 enabled; /* bit mask of slots in use */
    struct DirectionalController *slots;
    struct Player *actor;
    struct SceneVec position; /* copy of the actor's position */
};

struct DirectionalPackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};

struct DirectionalScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};

extern struct DirectionalManager *gDirectionalManager;
extern u32 gUnk_02000560;
extern u16 gPadState[];
extern void *gUnk_086049F4[];

/* Engine helpers. */
s32 Script_GetKeywordValue(u32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetPc(void);
s32 Script_GetValue(void);
s32 Region_FindContaining(struct SceneVec *);
s32 Math_Atan2(s32, s32);
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32 animation);
void Sprite_Remove(void *);
void Heap_Free(void *);
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
void Script_Call(u32, struct DirectionalScriptArgs *);
void sub_08249244(struct DirectionalManager *, struct DirectionalController *, s32, void *);

u32 DirectionalController_SetState(struct DirectionalController *, u32);
u32 DirectionalController_ConsumeEntered(struct DirectionalController *);
s32 DirectionalController_InitSlot(struct DirectionalManager *, struct DirectionalController *,
                                   s32);
struct DirectionalController *DirectionalController_FindByScriptId(struct DirectionalManager *, s32,
                                                                   s32 *);
u32 DirectionalController_SetMarkerFadeLevel(struct DirectionalController *, u32);
u32 DirectionalController_SetPosition(struct DirectionalManager *, struct DirectionalController *,
                                      struct DirectionalPackedVec *);
void LevelController_Update(struct DirectionalManager *, struct DirectionalController *);
s32 Actor_CanInteractWithController(struct DirectionalManager *, struct DirectionalController *);
void LevelController_RunScript(struct DirectionalManager *, struct DirectionalController *, u32);
u32 DirectionalController_SelectTarget(struct DirectionalManager *, s32);
u32 DirectionalController_ReleaseTarget(struct DirectionalManager *, s32);
s32 DirectionalController_CheckApproach(struct DirectionalManager *,
                                        struct DirectionalController *);
s32 DirectionalController_CheckInput(struct DirectionalManager *, struct DirectionalController *,
                                     s32);
s32 DirectionalController_CheckRemaining(struct DirectionalManager *,
                                         struct DirectionalController *);
s32 DirectionalController_UpdateAll(struct DirectionalManager *);
s32 DirectionalController_DestroyManager(struct DirectionalManager *);
s32 DirectionalController_InitManager(struct DirectionalManager *, void *);
void *DirectionalController_CreateManagerTask(void *);
s32 DirectionalController_ScriptCreate(void);
s32 DirectionalController_ScriptRelease(void);
s32 DirectionalController_ScriptMove(void);

#endif
