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
    u8 pad5[3];
    u8 entered;
    u8 pad9[2];
    u8 enabled;
    u8 padc[8];
    u32 ticks;
    u8 pad18[1];
    u8 facing4;
    u8 facing8;
    u8 pad1b[37];
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
    u8 nodeFacing;
    u8 pad227;
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
struct Node {
    u16 x, z;
    u8 pad4[2], flags;
};
int Path_GetPointNodeId(struct Node *, int);
int Path_GetPointFacing(struct Node *, int);
int Actor_SelectPathNodeAction(struct Actor *, struct Node *);
int SceneCharacter_GetPathNodeState(struct Actor *p)
{
    struct Path {
        u8 pad[8];
        struct Node *node;
    };
    struct Path *path = (struct Path *)p->path;
    struct Node *node;
    int dx, result;
    node = path->node;
    dx = node->x - p->pos.x;
    if (dx < 0)
        dx = -dx;
    if (dx <= 40) {
        int dz = node->z - p->pos.z;
        if (dz < 0)
            dz = -dz;
        if (dz <= 40)
            result = Actor_SelectPathNodeAction(p, node);
        else
            result = 9;
    } else
        result = 9;
    return result;
}
