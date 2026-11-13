#include "player_3.h"
#include "ram.h"

void Player_EmitQueuedEffects(struct Player *);
s32 Mod(s32, s32);
static inline int Player_NextStatusRandom(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gRandomTable);
}

static inline u32 Player_ReadStatusBonusBits(u32 *flags, u32 mask)
{
    u32 m = mask;
    return *flags & m;
}
static inline void Player_StoreStatusTimer(u16 *timers, s32 offset, u32 duration)
{
    *(u16 *)((u8 *)timers + offset) = duration;
}
void sub_0813D834(struct Player *p, s32 inputKind, u32 inputDuration)
{
    u32 kind = inputKind;
    u32 duration = inputDuration;
    s32 offset;
    if (kind == 1)
        goto bonus;
    switch (kind) {
        case 0: {
            u32 count = p->movementModifierActive;
            offset = 0;
            if (count && p->movementModifierKind != 3)
                Player_EmitQueuedEffects(p);
        } break;
        case 1:
        bonus:
            if (p) {
                u32 flags = Player_ReadStatusBonusBits(&p->statBonusFlags, 0x20000000);
                offset = 2;
                if (flags)
                    break;
            }
            {
                s32 scaled = kind * 2;
                u16 *timers = p->scriptStatus;
                u32 current = *(u16 *)((u8 *)timers + scaled);
                offset = scaled;
                if (!current) {
                    u16 *target = &gPlayerSavedMenuOption;
                    s32 random = Mod(Player_NextStatusRandom(), 7);
                    s32 selection = (s8)p->catalogSavedOption + 1;
                    selection += random;
                    selection &= 7;
                    *target = selection;
                    p->catalogSavedOption = selection;
                    Sound_PlaySong(0x138);
                }
            }
            break;
        default:
            return;
    }
    Player_StoreStatusTimer(p->scriptStatus, offset, duration);
}

