#include "global.h"
#include "ram.h"
struct Armor {
    u8 pad0[0x48];
    s16 x, y, z;
    u8 pad4E[0xb18 - 0x4e];
    u32 scriptValue;
    u8 padB1C[0xb2a - 0xb1c];
    s16 rewardId;
    u8 padB2C[2];
    s16 scriptParam;
    u8 padB30[8];
    u8 variant;
    u8 padB39[0xb4c - 0xb39];
    u32 defeatScript;
};
struct ArmorScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};
extern u32 gEngineFlags, gUnk_02000560;
extern u8 *gGameState;
extern const u8 gUnk_0824F004[];
void sub_08159164(s32);
void GameState_SetEventFlag(u32);
void Script_Call(u32, struct ArmorScriptArgs *);
void Task_Kill(void *);
#define H(p, o) (*(s16 *)((p) + (o)))
static inline u32 Armor_TestDefeatBlocked(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
void Armor_FinalizeDefeat(struct Armor *p)
{
    s32 values[8];
    struct ArmorScriptArgs args;
    u32 mask = 0x400;
    if (!((gEngineFlags | gScenePauseFlags) & mask) && !Armor_TestDefeatBlocked()) {
        s32 v;
        u32 script;
        sub_08159164(p->rewardId);
        H(gGameState, 0x87c) = H(gGameState, 0x87c) < 9999 ? H(gGameState, 0x87c) + 1 : 9999;
        H(gGameState, 0x538) = H(gGameState, 0x538) < 9999 ? H(gGameState, 0x538) + 1 : 9999;
        H(gGameState, 0x57e) = H(gGameState, 0x57e) < 9999 ? H(gGameState, 0x57e) + 1 : 9999;
        v = H(gGameState, 0x58c) + 1;
        if (v > 9999)
            v = 9999;
        else if (v < 0)
            v = 0;
        H(gGameState, 0x58c) = v;
        GameState_SetEventFlag(gUnk_0824F004[p->variant]);
        script = p->defeatScript;
        if (script) {
            args.count = 8;
            args.values = values;
            values[0] = p->x;
            values[1] = p->y;
            values[2] = p->z;
            values[3] = 0;
            values[4] = 0;
            values[5] = 0;
            values[6] = p->scriptParam;
            values[7] = p->scriptValue;
            Script_Call(script, &args);
        }
    }
    Task_Kill(p);
}
