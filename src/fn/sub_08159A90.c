#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
s32 PlayerContact_ApplySyntheticContact(struct Player *, s32, u32, u32, u32, u32);
s32 Player_ApplyPositionRequest(struct Player *p, struct SceneVec *position, s32 frame, u32 direction,
                 s32 resetValue, s32 parameter, s32 request, u32 option)
{
    s32 result;
    if (p->kind != 1 || p->tetherMode != 0x18)
        return -1;
    if (parameter > 0) {
        p->movementRequestParameter = parameter;
        p->movementRequestScale = 0x40;
    }
    p->position = *position;
    direction = ((s32)((direction + 0xE0) & 0xFF) >> 6) * 2 + 1;
    p->facing = direction;
    if (direction > 4) {
        p->heading = 8 - direction;
        p->side = 1;
    } else {
        p->heading = direction;
        p->side = 0;
    }
    p->directionFrame = frame;
    p->contactSpecial = 1;
    Sound_PlaySong(0xE6);
    if (p->positionResetTimer)
        p->positionResetTimer = 0;
    p->terrainClampApplied = 0;
    p->actionPhase488 = 0;
    p->terrainRecoveryPhase = 0;
    p->terrainClampBlocked = 0;
    p->actionActive489 = 0;
    p->terrainRecoveryActive = 0;
    if (request > 0)
        result = PlayerContact_ApplySyntheticContact(p, request, 0, 0, 0, option);
    else
        result = 0;
    if (resetValue > 0)
        p->positionResetValue = resetValue;
    return result;
}
