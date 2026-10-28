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
    u8 frame;
    u8 event, pad25[3];
    u32 buffer, count[2];
    u8 pad34[0x400];
    struct Vec *target;
    struct Player *player;
};
extern struct Player *gUnk_02000580[4];
int Weather_GetType(void);
int SunClock_GetBaseHour(void);
int SunClock_GetBaseMinute(void);
void Actor_RunCallbacks(void *, struct Actor *);
void sub_0802E188(struct Manager *);
void sub_0802E04C(void *);
int Actor_Destroy(struct Actor *);
int SceneActorManager_Update(struct Manager *m)
{
    struct Player *player = gUnk_02000580[0];
    int zero;
    u8 *alias;
    m->player = player;
    if (player) {
        m->target = &player->pos;
    } else {
        m->target = 0;
    }
    m->event = Weather_GetType();
    m->phase = SunClock_GetBaseHour();
    {
        int frame = SunClock_GetBaseMinute();
        u8 *current = &m->frame;
        u8 *previous;
        int old;
        u8 *keep;
        *current = frame;
        previous = &m->previous;
        old = *previous;
        frame = (u8)frame;
        keep = previous;
        alias = current;
        if (old != frame) {
            *(previous - 1) = 1;
        } else {
            m->changed = 0;
        }
        {
            int value = *alias;
            zero = 0;
            *keep = value;
        }
    }
    {
        u32 buffer = 1 - m->buffer;
        m->buffer = buffer;
        m->count[buffer] = zero;
    }
    {
        struct Actor *p = m->head;
        while (p) {
            struct Actor *prev = p->prev;
            Actor_RunCallbacks(m, p);
            p = prev;
        }
    }
    sub_0802E188(m);
    if (m->player) {
        sub_0802E04C(((u8 *)m->player) + 0x24);
    }
    m->ticks++;
    return 0;
}
