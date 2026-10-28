#include "global.h"
#include "ram.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Player {
    u8 pad[0x30];
    struct Vec pos;
};
struct Actor {
    u8 pad[0x2e0];
    void (*destroy)(void *, struct Actor *);
    u8 pad2e4[8];
    struct Actor *prev;
};
struct Manager {
    u8 pad0[0x18];
    u32 ticks;
    struct Actor *head;
    u8 changed, previous, phase;
    volatile u8 frame;
    u8 event, pad25[3];
    u32 buffer, count[2];
    u8 pad34[0x400];
    struct Vec *target;
    struct Player *player;
};
extern struct Player *gPlayers[4];
int Weather_GetType(void);
int SunClock_GetBaseHour(void);
int SunClock_GetBaseMinute(void);
void Actor_RunCallbacks(void *, struct Actor *);
void sub_0802E188(struct Manager *);
void sub_0802E04C(void *);
int Actor_Destroy(struct Actor *);
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int SceneActorManager_Init(struct Manager *);
void Task_Kill(void *);
void SceneActorManager_Update(void);
void SceneActorManager_Destroy(void);
/* Registry routine 286F: returns the scene actor manager (gSceneActorListManager,
   which also holds the message packet queue), creating its task on first use. */
struct Manager *SceneActorManager_Create(void)
{
    struct Manager *p;
    if (gSceneActorListManager)
        return gSceneActorListManager;
    p = Task_Create(9, 0x43c);
    if (p) {
        Task_SetCallbacks(p, SceneActorManager_Update, SceneActorManager_Destroy);
        if (SceneActorManager_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
