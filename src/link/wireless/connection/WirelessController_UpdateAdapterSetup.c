#include "global.h"
#include "link/wireless/wireless_controller.h"
void Memory_Clear(void *, int);
#include "lib/rfu/rfu_compat.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "engine/math/monster_particle_random.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->operationStage = zero;
    p->operation = op;
    p->operationFrame = zero;
    p->operationEntered = 1;
}
int Wireless_CheckAdapterReady(void);
int Script_ParseStringRef(const void *);
const u8 *Text_LookupString(int);
int SunClock_GetPhase(void);
int SunClock_GetBaseHour(void);
void WirelessText_EncodeString(const void *, void *, int);
void Wireless_InitSessionConfig(void *, int, int, int, int, const void *, const void *);
struct Game {
    u8 pad[0x780];
    u8 playerName[8];
};
static inline u16 Random_Next(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
int WirelessController_UpdateAdapterSetup(struct Engine *p)
{
    if (p->operationEntered)
        p->operationEntered = 0;
    if (p->adapterDelay) {
        p->adapterDelay--;
        return 0;
    }
    switch (Wireless_CheckAdapterReady()) {
        case -1:
            setOperation(p, 27, 0);
            break;
        case 0:
            p->adapterDelay = 60;
            break;
        case 1: {
            int base;
            int offset;
            int chance;
            const u16 *table;
            union ParticleRandomTableSource source;
            const u8 *name;
            u8 *userName;
            u8 *gameName;
            setOperation(p, 3, 0);
            base = Script_ParseStringRef(p->gameNameScript);
            source.halves[0] = (u16)RANDOM_TABLE_ADDR;
            source.halves[1] = RANDOM_TABLE_ADDR >> 16;
            table = ParticleRandom_GetTable(&source);
            chance = (u8)Random_Next(table);
            if (chance <= 63) {
                u32 form = SunClock_GetPhase();
                int stage = SunClock_GetBaseHour();
                if (form - 1 <= 2) {
                    offset = 1;
                    if (stage <= 11)
                        offset = 0;
                } else {
                    offset = 3;
                    if (stage <= 11)
                        offset = 2;
                }
            } else
                offset = (Random_Next(table) & 3) + 4;
            name = Text_LookupString(base + offset);
            WirelessText_EncodeString(gGameState->playerName, p->userName, 8);
            userName = p->userName;
            gameName = p->gameName;
            WirelessText_EncodeString(name, gameName, 11);
            p->gameIdLow = p->gameId;
            p->gameIdHigh = p->gameId >> 8;
            {
                void *config = p->rfuConfig;
                int mode = p->isParent;
                int flags = 0;
                if (mode)
                    flags = 3;
                Wireless_InitSessionConfig(config, mode, 43, flags, 0, gameName, userName);
            }
            break;
        }
        default:
            break;
    }
    return 0;
}
