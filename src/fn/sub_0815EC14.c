#include "global.h"
#include "player.h"
#include "ram.h"
/* Byte view preserves the original global-pointer reloads between stores. */
extern u8 *gGameState;
extern u16 gUnk_02000534, gUnk_02000554;
extern u16 gUnk_02000548[];
extern u32 gUnk_02000560[];
void Player_SaveStateSnapshot(struct Player *p)
{
    s32 i;
    *(u16 *)(gGameState + 0x30) = p->position.x;
    *(u16 *)(gGameState + 0x32) = p->position.y;
    *(u16 *)(gGameState + 0x34) = p->position.z;
    *(u16 *)(gGameState + 0x36) = p->facing;
    *(s16 *)(gGameState + 0x85e) = ((p->facing + 5) & 7) << 5;
    p->unk2D = (u16) * (s16 *)(gGameState + 0x85e);
    {
        s32 heading = *(s16 *)(gGameState + 0x85e);
        p->savedHeading = heading;
    }
    *(u16 *)(gGameState + 0x5a8) = p->actionMode;
    *(u16 *)(gGameState + 0x85c) = p->tetherMode;
    if (p->actionMode == 6) {
        *(u16 *)(gGameState + 0x38) = p->currentStatA;
        *(u16 *)(gGameState + 0x3a) = p->currentStatB;
    } else {
        *(u16 *)(gGameState + 0x28) = p->currentStatA;
        *(u16 *)(gGameState + 0x2c) = p->currentStatB;
    }
    *(u16 *)(gGameState + 0x2a) = p->maximumStatA;
    *(u16 *)(gGameState + 0x2e) = p->maximumStatB;
    *(u32 *)(gGameState + 0x608) = p->statHundredths;
    *(u32 *)(gGameState + 0x60c) = 0;
    gPlayerInteractionMode = p->sceneMode;
    gPlayerScriptState = p->unk536;
    gUnk_02000534 = p->active;
    for (i = 0; i < 2; i++)
        gUnk_02000548[i] = p->scriptStatus[i];
    gUnk_02000560[p->movementRequestBlocked] = p->kind;
    gUnk_02000554 = p->actionSelectionStamp;
}
