// MATCHING_CONCESSION: register-pinning
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
int Actor_EndScriptControl(void)
{
    void *m = gSceneActorListManager;
    struct Actor *p;
    int mode;
    struct Actor **alias;
    if (!m) {
        return -1;
    }
    p = Actor_FindById(m, Script_GetKeywordValue(0x6e, 0));
    if (!p) {
        return -1;
    }
    {
        u8 *controlled = &p->controlled;
        int zero;
        zero = 0;
        *controlled = zero;
        mode = Script_GetKeywordValue(0x69, 0);
        {
            u32 command = mode;
            alias = &p;
            switch (command) {
                default:
                    break;

                case 0:
                    /* if (1) only keeps agbcc's block layout */
                    if (1) {
                    }
                    {
                        int one = 1;
                        (*alias)->stage = command;
                        (*alias)->phase = command;
                        p->state = one;
                        p->ticks = command;
                        p->entered = one;
                        break;
                    }

                case 1:
                    (*alias)->stage = zero;
                    (*alias)->phase = zero;
                    (*alias)->state = zero;
                    p->ticks = zero;
                    (*alias)->entered = command;
                    break;
                    return mode;
            }
        }
    }
}
