#include "global.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
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
int Actor_BeginScriptControl(void)
{
    void *m = gSceneActorListManager;
    struct Actor *p;
    if (!m)
        return -1;
    p = Actor_FindById(m, Script_GetKeywordValue(0x6e, 0));
    if (!p)
        return -1;
    if (Script_SeekToKeyword(0x64)) {
        int value = Script_GetValue();
        register int angle asm("r0");
        p->angle = value;
        angle = (u16)value;
        {
            register int shifted asm("r2") = (angle + 112) >> 5;
            int facing = 7 & shifted;
            p->facing8 = facing;
        }
        angle = (angle + 96) >> 6;
        {
            int facing = 3 & angle;
            p->facing4 = facing;
        }
    }
    if (Script_SeekToKeyword(0x72)) {
        p->pathId = Script_GetValue();
        p->pathParamA = Script_GetValue();
        p->pathParamB = Script_GetValue();
        SceneBody_StartRoute(&p->body, p->path, (u8)p->pathId, (u8)p->pathParamA,
                             (u8)p->pathParamB);
        Path_GetPointPosition(&p->pos, p->map, p->pathParamB);
    }
    if (Script_SeekToKeyword(0x62)) {
        p->modeA = Script_GetValue();
        p->modeB = Script_GetValue();
        p->modeValue = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x74))
        p->script = Script_GetValue();
    if (Script_SeekToKeyword(0x48)) {
        p->turnA = Script_GetValue();
        p->turnB = Script_GetValue();
        p->turnState = 255;
    }
    if (Script_SeekToKeyword(0x75))
        p->enabled = Script_GetValue();
    {
        register int state asm("r0");
        int zero;
        if (p->pathId >= 0)
            state = SceneCharacter_GetPathNodeState(p);
        else
            state = 6;
        zero = 0;
        p->stage = zero;
        p->phase = zero;
        p->state = state;
        p->ticks = zero;
        {
            int one = 1;
            asm("" : "+r"(one));
            p->entered = one;
        }
        {
            u8 *controlled = &p->controlled;
            *controlled = 1;
        }
    }
    return 0;
}
