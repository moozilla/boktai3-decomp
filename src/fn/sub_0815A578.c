#include "global.h"

s32 Script_GetKeywordValue(s32, s32);
struct Actor {
    u8 pad[0x424];
    u16 lifeMax, life, energyMax, energy;
};
extern struct Actor *gUnk_02000580[];
extern u8 *gGameState;
s32 Script_GetActorIndex(void);
/* Script: life -l and energy -e (current and maximum) of the scripted actor
   picked by -i, mirrored into game state +0x28..+0x2E (tentative). Raw
   offset stores: the original reloads the game state pointer per store. */
void ScriptActor_SetLifeEnergyFromScript(void)
{
    s32 i = Script_GetActorIndex();
    struct Actor *a = gUnk_02000580[i];
    if (a) {
        a->life = Script_GetKeywordValue('l', 0);
        a->energy = Script_GetKeywordValue('e', 0);
        a->lifeMax = a->life;
        a->energyMax = a->energy;
        *(u16 *)(gGameState + 0x28) = a->life;
        *(u16 *)(gGameState + 0x2C) = a->energy;
        *(u16 *)(gGameState + 0x2A) = a->life;
        *(u16 *)(gGameState + 0x2E) = a->energy;
    }
}
