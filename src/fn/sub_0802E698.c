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
    u8 pad224[4];
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
void Actor_RegisterMotion(struct Actor *);
int Actor_InitBehaviorFromScript(void *unused, struct Actor *p)
{
    if (Script_GetKeywordValue(0x43, 1))
        Actor_RegisterMotion(p);
    {
        u8 angle = p->angle;
        p->savedAngle = angle;
    }
    p->registered = Script_GetKeywordValue(0x43, 1);
    if (Script_SeekToKeyword(0x72)) {
        p->pathId = Script_GetValue();
        p->pathParamA = Script_GetValue();
        p->pathParamB = Script_GetValue();
        SceneBody_StartRoute(&p->body, p->path, (u8)p->pathId, (u8)p->pathParamA,
                             (u8)p->pathParamB);
        Path_GetPointPosition(&p->pos, p->map, p->pathParamB);
    } else
        p->pathId = -1;
    if (Script_SeekToKeyword(0x68)) {
        p->hasDestination = 1;
        p->destination.x = Script_GetValue();
        p->destination.y = Script_GetValue();
        p->destination.z = Script_GetValue();
    } else {
        p->hasDestination = 0;
        p->destination = p->pos;
    }
    if (Script_SeekToKeyword(0x54)) {
        p->hasParameters = 1;
        p->paramA = Script_GetValue();
        p->paramB = Script_GetValue();
        p->paramC = Script_GetValue();
        p->paramD = Script_GetValue();
    } else {
        int zero = 0;
        p->hasParameters = zero;
        p->paramA = 9;
        p->paramB = zero;
        p->paramC = 21;
        p->paramD = zero;
    }
    if (Script_SeekToKeyword(0x62)) {
        p->modeA = Script_GetValue();
        p->modeB = Script_GetValue();
        p->modeValue = Script_GetValue();
    } else {
        int zero = 0;
        p->modeA = zero;
        p->modeB = 1;
        p->modeValue = zero;
    }
    if (Script_SeekToKeyword(0x77)) {
        p->rateC = Script_GetValue();
        p->rateA = Script_GetValue();
        p->rateB = Script_GetValue();
    } else {
        int *dst = &p->rateC;
        int rate;
        rate = 256;
        *dst = rate;
        p->rateA = rate;
        p->rateB = 512;
    }
    if (Script_SeekToKeyword(0x48)) {
        p->turnA = Script_GetValue();
        p->turnB = Script_GetValue();
        p->turnRate = 4;
        p->turnState = 255;
    } else {
        u8 *dst = &p->turnA;
        int six;
        six = 6;
        *dst = six;
        p->turnB = six;
        p->turnRate = 4;
        p->turnState = 255;
    }
    p->turnRate = Script_GetKeywordValue(0x42, 4);
    if (Script_SeekToKeyword(0x74))
        p->script = Script_GetValue();
    if (Script_SeekToKeyword(0x73)) {
        int a = Script_GetValue();
        int b = Script_GetValue();
        p->radiusA = a;
        p->radiusB = b;
        p->radiusA32 = a;
        p->radiusB32 = b;
        p->radiusASquared = a * a;
        p->radiusBSquared = b * b;
    } else {
        int a = 80, b = 96;
        p->radiusA = a;
        p->radiusB = b;
        p->radiusA32 = a;
        p->radiusB32 = b;
        p->radiusASquared = 6400;
        p->radiusBSquared = 9216;
    }
    p->distance = Script_GetKeywordValue(0x4d, 10);
    p->distanceSquared = p->distance * p->distance;
    p->enabled = Script_GetKeywordValue(0x75, 1);
    p->soundEnabled = Script_GetKeywordValue(0x46, 1);
    return 0;
}
