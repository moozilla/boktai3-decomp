#define Player_IsActionRequestEnabled Player_IsEnchantmentAvailable
#include "global.h"
#include "player.h"
s32 ActorParameter_GetModeValue(s32);
s32 Player_IsEnchantmentAvailable(struct Player *, s32);
void PlayerBarrier_Hide(void *);
s32 PlayerBarrier_IsActive(void *);
static inline void ClearModeFlag(struct Player *p)
{
    if (p) {
        u32 *flags = &p->actionStateFlags;
        u32 mask = ~16;
        *flags &= mask;
    }
}
static inline void SetModeFlag(struct Player *p)
{
    if (p) {
        u32 *flags = &p->actionStateFlags;
        u32 mask = 16;
        *flags |= mask;
    }
}
void Player_SetActionRequest(struct Player *p, s32 kind)
{
    struct PlayerActionRequest *request = (struct PlayerActionRequest *)&p->thresholdMode;
    if (request->kind != kind) {
        if (request->latched) {
            request->latched = 0;
            p->actionVariant = 0;
        }
        request->kind = kind;
        ClearModeFlag(p);
        if (kind == 3)
            SetModeFlag(p);
        else if (PlayerBarrier_IsActive(p->actionRequestEffect))
            PlayerBarrier_Hide(p->actionRequestEffect);
        if ((u8)request->kind > 5) {
            request->latched = 0;
            request->enabled = 0;
            request->duration = 0;
            p->actionRequestState = 0;
            if (p->actionRequestHold == 0)
                p->actionRequestTimer = 0;
        } else {
            request->enabled = Player_IsEnchantmentAvailable(p, request->kind);
            request->duration = ActorParameter_GetModeValue(request->kind);
            if (!request->enabled)
                return;
            request->latched = 0;
            p->actionRequestState = 0;
            if (p->actionRequestHold == 0)
                p->actionRequestTimer = 0;
        }
        p->actionVariant = p->baseActionVariant;
    }
}
