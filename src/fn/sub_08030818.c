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
int SceneActorManager_Init(struct Manager *m)
{
    int zero;
    struct Player *player;
    gSceneActorListManager = m;
    zero = 0;
    m->ticks = zero;
    m->head = zero;
    m->buffer = zero;
    m->count[0] = zero;
    m->count[1] = zero;
    player = gPlayers[0];
    m->player = player;
    if (player)
        m->target = &player->pos;
    else
        m->target = 0;
    return 0;
}
