#include "global.h"
extern u32 gParticipantActors[];
u32 Script_GetKeywordValue(u32, u32);
void StageIconDisplay_Create(u32, u32, void (*)(void), u32);
void Glove_StopWithMessage(void);
void Glove_CreateStageIconFromScript(void)
{
    u32 p;
    u32 i = Script_GetKeywordValue(0x69, 0);
    p = gParticipantActors[i];
    if (p != 0)
        StageIconDisplay_Create(0, 0, Glove_StopWithMessage, p);
}
