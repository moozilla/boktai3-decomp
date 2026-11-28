#include "global.h"
#include "engine/ram.h"
#include "link/lobby/record_menu.h"

void LinkLobbyPanel_RemoveSprites(struct RecordMenu *);
void LinkLobbyPanel_ClearTextWindows(struct RecordMenu *);
void CharacterModel_RemoveAnimations(void *);

s32 LinkLobbyPanel_OnTaskDestroy(struct RecordMenu *menu)
{
    s32 i;

    LinkLobbyPanel_RemoveSprites(menu);
    LinkLobbyPanel_ClearTextWindows(menu);
    for (i = 0; i < 2; i++)
        CharacterModel_RemoveAnimations(&menu->previews[i].model);
    gActiveRecordMenu = 0;
    return 1;
}
