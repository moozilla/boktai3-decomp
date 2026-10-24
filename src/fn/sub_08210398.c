// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining: pause branch layout, flags allocation and the defined callback return differ.
#include "pink_spine_playback.h"
// confidence=low: prefix follows the neighboring creator; species identity remains tentative.
extern u32 gUnk_030053F0, gUnk_02000560;
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void ProjectileEmitter_EmitAttackFrame(void);
void ProjectileEmitter_UpdateAttack(void);
static inline u32 PauseFlags(u32 mask)
{
    return (gUnk_030053F4 | gUnk_030053F0) & mask;
}
static inline u32 TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
s32 PinkSpine_UpdateDamageAnimation(struct PinkSpineState *p)
{
    s32 t1;
    t1 = 4;
    if (PauseFlags(0x400) || TestFlags(&gUnk_02000560, t1))
        return 0;
    Callback_InvokeActor(p, p->update);
    p->sprite.flags &= ~1;
    if (!p->f134) {
        struct ColliderView {
            u8 pad[6];
            u16 flags;
        } *c = (void *)(&p->collider);
        c->flags &= ~4;
        Collider_SetPosition(c, &p->sprite.position, 0);
    } else {
        struct ColliderView {
            u8 pad[6];
            u16 flags;
        } *c = (void *)(&p->collider);
        c->flags |= 4;
    }
    if ((p->f130 > 0) && (!p->f134)) {
        p->f136 -= p->f130;
        p->f144 = p->f148;
        Sound_PlaySong(0x13E);
        if (p->f136 <= 0) {
            p->f134 = 1;
            Anim_RestartSequence(&p->sequence, p->f15C, 0xD, 0, 0);
            AnimData_SelectPaletteBank(p->graphics, 0x1BD);
            p->f12C = 0;
            p->update = ProjectileEmitter_EmitAttackFrame;
            Sound_PlaySong(0x522);
            p->age = 0;
        }
        p->f12C = 4;
        p->f132 = 1;
        p->f130 = 0;
    }
    if (p->f12C > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->f12C--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1BD);
    {
        register u32 allocationToken asm("r0");
        u32 ended;
        ended = PinkSpine_StepAnimation(p);
        if (ended && (p->update == ProjectileEmitter_UpdateAttack))
            Sound_PlaySong(0x521);
        asm("" : : "r"(allocationToken));
    }
    return 0;
    p->age++;
}

#else
INCLUDE_ASM("asm/nonmatching", PinkSpine_UpdateDamageAnimation);
#endif
