#include "global.h"
#include "game_state.h"
struct SceneCatalog {
    u8 unknown0[0x60];
    const s32 *levelValues;
};
s32 SceneCatalog_GetLevelScaledValue(struct SceneCatalog *s)
{
    const s32 *values = s->levelValues;
    values += gGameState->level;
    return *values * 10;
}
