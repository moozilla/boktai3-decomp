#include "global.h"
#include "ram.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 active;
    u8 pad1[1];
    u8 stage;
    u8 phase;
    u8 state;
    u8 action, pad6, done;
    u8 entered;
    u8 changed, flashing;
    u8 enabled;
    u8 padc[8];
    u32 ticks;
    u8 pad18[1];
    u8 facing4;
    u8 facing8;
    u8 targetAngle;
    u8 pad1c[36];
    u32 script;
    u8 pad44[24];
    u32 body;
    u8 pad60[5];
    u8 angle;
    u8 pad66[2];
    struct Vec pos;
    u8 pad70[300];
    u8 path[8];
    void *map;
    u8 pad1a8[68];
    u8 modeA;
    u8 modeB;
    u16 modeValue;
    u8 pad1f0[1];
    u8 hasDestination;
    u8 pad1f2[1];
    s8 pathId;
    s16 radiusA;
    s16 radiusB;
    int radiusA32;
    int radiusB32;
    int radiusASquared;
    int radiusBSquared;
    int distance;
    int distanceSquared;
    u16 pathParamA;
    u8 pad212[2];
    u16 pathParamB;
    u8 soundEnabled;
    u8 pad217[2];
    u8 savedAngle;
    u8 registered;
    u8 controlled;
    int rateA;
    int rateB;
    u8 pad224[2];
    u8 nodeFacing, savedState;
    int rateC;
    u8 pad22c[4];
    u8 hasParameters;
    u8 paramA;
    u8 paramB;
    u8 paramC;
    u8 paramD;
    u8 pad235[3];
    u8 turnA;
    u8 turnB;
    u8 turnRate;
    u8 turnState;
    struct Vec destination;
};
struct Actor *Actor_FindById(void *, int);
int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
int SceneCharacter_GetPathNodeState(struct Actor *);
void SceneBody_StartRoute(void *, void *, int, int, int);
void Path_GetPointPosition(void *, void *, int);
struct Context {
    u8 pad[0x434];
    struct Vec *target;
};
static inline int near(struct Actor *p, struct Vec *b)
{
    struct Vec *a = &p->pos;
    int limit;
    int x;
    int result;
    limit = p->rateA;
    x = b->x - a->x;
    if (x < 0)
        x = -x;
    if (x >= limit)
        goto fail;
    {
        int y = b->y - a->y;
        if (y < 0)
            y = -y;
        if (y > 255)
            goto fail;
    }
    {
        int z = b->z - a->z;
        if (z < 0)
            z = -z;
        result = 1;
        if (z < limit)
            goto done;
    }
fail:
    result = 0;
done:
    return result;
}
static inline int targetNear(struct Context *m, struct Actor *p)
{
    int result;
    if (m->target) {
        int close = near(p, m->target);
        result = 1;
        if (close)
            goto done;
    }
    result = 0;
done:
    return result;
}
void SceneCharacter_UpdatePathTurnState(struct Context *m, struct Actor *p)
{
    int close;
    if (p->entered) {
        int zero = 0;
        p->entered = zero;
        p->done = zero;
        p->action = 4;
        p->targetAngle = (p->nodeFacing + 5) << 5;
    }
    close = targetNear(m, p);
    if (close) {
        int state = p->state;
        u32 offset = 0x227;
        u8 *dst = (u8 *)p + offset;
        int zero;
        zero = 0;
        *dst = state;
        p->state = 10;
        p->ticks = zero;
        p->entered = 1;
    } else if (p->done) {
        p->state = 7;
        p->ticks = close;
        p->entered = 1;
    } else
        p->ticks++;
}
