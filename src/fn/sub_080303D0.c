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
    void *owner;
};
struct Body {
    u8 pad[12];
    struct Vec pos;
};
static inline int near(struct Actor *p, struct Vec *b, struct Body *body)
{
    struct Vec *a = &p->pos;
    int limit = p->rateB;
    int x = b->x - body->pos.x;
    int result;
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
void SceneCharacter_UpdateTargetProximityState(struct Context *m, struct Actor *p)
{
    struct Body *body = (struct Body *)&p->body;
    if (p->entered) {
        int zero = 0;
        p->entered = zero;
        p->done = zero;
        p->action = 9;
    }
    if (m->owner && m->target) {
        int close = near(p, m->target, body);
        if (!close) {
            int registered = p->registered;
            if (registered) {
                p->targetAngle = p->savedAngle;
                p->state = 3;
                p->ticks = close;
            } else {
                p->state = p->savedState;
                p->ticks = registered;
            }
            p->entered = 1;
        } else
            p->ticks++;
    }
}
