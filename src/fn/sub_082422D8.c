#include "global.h"
#include "player.h"
/* Chooses the player's input-driven action mode for this update (tentative). */
u32 Player_GetTetherMode(struct Player *);
s32 Trigger_TryOpenNearby(void *);
void Player_StopLocalActionSounds(struct Player *);
void Player_ClearMovementLock(struct Player *);
s32 Player_TryStepForward(struct Player *);

static inline s32 IsScene5(struct Player *p)
{
    if (p->active != 0 && p->sceneMode == 5)
        return 1;
    return 0;
}

u32 Player_SelectInputMode(struct Player *p)
{
    u32 mode = p->tetherMode;
    const u16 *keys;
    if (p == NULL || !IsScene5(p)) {
        if (p->inputKeys[1] & 0x200)
            mode = Player_GetTetherMode(p);
    }
    if (p->directionFrame != 0) {
        u32 c = p->contactSpecial;
        mode = 0x1d;
        if (c != 0)
            mode = 0x1e;
        Player_StopLocalActionSounds(p);
        Player_ClearMovementLock(p);
        return mode;
    }
    if (mode - 0x1d > 1 && mode != 0x1b && mode != 0x1c && mode != 0x18 && mode != 0x17 &&
        mode != 0x16 && p->positionResetTimer != 0)
        mode = 0x15;
    switch (mode) {
        case 2:
        case 3:
        case 7:
        case 14:
        case 16:
        case 18:
        case 27:
        case 28:
        case 29:
        case 30:
            break;
        case 4:
            if (Player_TryStepForward(p) != 0)
                break;
        case 5:
        case 6:
        case 8:
        case 9:
        case 10:
        case 12:
        case 13:
        case 15:
        case 17:
        case 21:
        case 22:
        case 23:
        case 24:
        case 37:
        zero:
            mode = 0;
            break;
        default:
            if (p && IsScene5(p)) {
                if ((s16)p->directionHistory[0] == -1)
                    goto zero;
                if (p->inputKeys[0] & 0x100)
                    goto zero;
                mode = 1;
            } else {
                keys = p->inputKeys;
                if (keys[1] & 2) {
                    mode = 3;
                    break;
                }
                if (keys[1] & 1) {
                    s32 t = Trigger_TryOpenNearby(&p->positionalEffectGroup) << 24;
                    mode = 7;
                    if (t == 0)
                        break;
                    goto zero;
                }
                if ((s16)p->directionHistory[0] == -1)
                    goto zero;
                if (keys[0] & 0x100)
                    goto zero;
                mode = 1;
            }
            break;
    }
    return mode;
}
