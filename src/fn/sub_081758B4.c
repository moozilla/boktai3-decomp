#include "global.h"
#include "game_state.h"
#include "player.h"
struct SceneCatalog {
    u8 unknown0[0xA40];
    struct Player *player;
    u8 unknownA44[0x3E7D];
    u8 optionA, optionB, optionC;
};
s32 SceneCatalog_MapSelectionMask(s32);
void GameState_BackupRegionRange(void *, u32);
void SceneCatalog_SaveOptions(struct SceneCatalog *s)
{
    struct Player *player;
    s32 mapped;
    gGameState->values08[4] = s->optionA;
    mapped = SceneCatalog_MapSelectionMask(s->optionB);
    *(u16 *)((u8 *)gGameState + 0x12) = mapped;
    *(u16 *)((u8 *)gGameState + 0x14) = s->optionC;
    GameState_BackupRegionRange(&gGameState->values08[4], 2);
    GameState_BackupRegionRange(&gGameState->unk12, 2);
    GameState_BackupRegionRange(&gGameState->unk14, 2);
    player = s->player;
    if (player->scriptStatus[1] == 0)
        player->catalogSavedOption = gGameState->values08[4];
}
