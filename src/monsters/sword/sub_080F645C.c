#include "monsters/ivy/phase_object.h"
#include "monsters/sword/sword_task.h"

void Sword_UpdateBodyPaletteRestore(void *);
void ChildGroup_ShowDamageNumber(void *, u32, u32);
void Ivy_UpdateDefeatBurst(void);
#define H(p, n) (*(s16 *)((u8 *)(p) + (n)))
static inline s32 clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}

static inline s32 SubtractDamage(u16 *health, u16 *damage)
{
    return *health - *damage;
}
void Sword_UpdateDamage(struct SwordTask *p)
{
    u32 zero;
    if (p->body.responseValue)
        p->body.responseValue--;
    Sword_UpdateBodyPaletteRestore(p);
    if (p->damageKind) {
        {
            s32 health = SubtractDamage((u16 *)&p->health, &p->damage);
            zero = 0;
            p->health = health;
        }
        if (p->health < 0) {
            p->damage += p->health;
            p->health = zero;
        }
        ChildGroup_ShowDamageNumber(p, (u16)p->damageKind, 0);
        p->damageKind = zero;
        p->damage = zero;
        if (p->health <= 0) {
            do {
                u32 flags;
                PhaseObject_EnterMode((void *)p, 1, 6);
                p->update = Ivy_UpdateDefeatBurst;
                H(gGameState, 0x87C) =
                    H(gGameState, 0x87C) < 9999 ? H(gGameState, 0x87C) + 1 : 9999;
                {
                    u8 *g = (u8 *)gGameState;
                    s16 *q;
                    u32 v;
                    q = (s16 *)(g + 0x538);
                    v = *(u16 *)q;
                    *q = *q < 9999 ? v + 1 : 9999;
                }
                flags = p->body.reactionFlags;
                do {
                    s16 *counter = (s16 *)((u8 *)gGameState + 0x548);
                    *counter = *counter < 9999 ? *counter + 1 : 9999;
                    if (flags & 0x80) {
                        gGameState->targetFlagBonuses[0] =
                            clamp(gGameState->targetFlagBonuses[0] + 1);
                    } else if (flags & 0x100) {
                        gGameState->targetFlagBonuses[1] =
                            clamp(gGameState->targetFlagBonuses[1] + 1);
                    } else if (flags & 0x200) {
                        gGameState->targetFlagBonuses[2] =
                            clamp(gGameState->targetFlagBonuses[2] + 1);
                    } else if (flags & 0x400) {
                        gGameState->targetFlagBonuses[3] =
                            clamp(gGameState->targetFlagBonuses[3] + 1);
                    } else if (flags & 0x800) {
                        gGameState->targetFlagBonuses[4] =
                            clamp(gGameState->targetFlagBonuses[4] + 1);
                    }
                } while (0);
            } while (0);
        }
    }
}
