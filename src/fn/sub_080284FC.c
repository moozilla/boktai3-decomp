// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 96.2%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: Vampire pending-damage handler: applies queued damage, flash/shake, death and stagger transitions.
struct VampireDamage {
    u8 pad0[0x48];
    u8 sprite[0x14];
    u8 pad5C[0x34];
    u32 timer;
    s16 health, maxHealth;
    u8 pad98[2];
    u8 healthCheck, pad9b;
    u32 flags;
    u8 padA0[2];
    u8 reset;
    u8 padA3;
    u32 pending;
    u8 padA8[3];
    u8 state;
    u8 padAC[5];
    u8 changed;
    u8 padB2[0x4e];
    void (*callback)(void *);
};
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void SceneActors_SpawnDestructionParticles(void *);
int SceneActorGraphics_GetEffectState(void *);
void sub_08026FF0(void *);
void sub_08027214(void *);
extern u32 gUnk_03005308;

static inline void Vampire_SetState(struct VampireDamage *p, void (*fn)(void *), int id)
{
    int zero;
    p->state = id;
    zero = 0;
    p->callback = fn;
    p->timer = zero;
    p->changed = 1;
    p->reset = zero;
    p->pending = zero;
}

static inline void Vampire_SetState_1(struct VampireDamage *p, void (*fn)(void *))
{
    int zero;
    p->state = 24;
    zero = 0;
    p->callback = fn;
    p->timer = zero;
    p->changed = 1;
    p->reset = zero;
    p->pending = zero;
}

int Vampire_ApplyPendingDamage(struct VampireDamage *p)
{
    int special;
    struct VampireDamage *base = p;
    u8 *sprite = ((u8 *)p) + 0x48;
    void *collider = ((u8 *)p) + 0x1cc;
    int damage;
    int trigger;
    int flaggedBit;
    u32 *flags;
    u8 *hit;
    s16 *hp;
    struct Packed size;
    u32 t1;
    {
        s32 *damagePtr = (s32 *)(((u8 *)p) + 0x33c);
        damage = *damagePtr;
        {
            u8 *specialPtr = ((u8 *)p) + 0x340;
            int value;
            value = *specialPtr;
            special = value;
            *damagePtr = 0;
            {
                u32 off = 0x341;
                hit = ((u8 *)p) + off;
            }
            *hit = 0;
            *specialPtr = 0;
        }
    }
    flaggedBit = ((u8 *)p)[0x20f] >> 3;
    flaggedBit &= 1;
    if ((((*((u32 *)(((u8 *)p) + 0x3ac))) & 1) && (((u8 *)p)[0x384] == 0)) &&
        ((*((s16 *)(((u8 *)gUnk_02000710) + 0x876))) > 0)) {
        *hit = 1;
        damage = p->health - (p->maxHealth / 2);
        if (damage < 0)
            damage = 0;
    }
    {
        u32 mask = 0x100000;
        u32 *fp = &base->flags;
        u32 test = (*fp) & mask;
        flags = fp;
        if ((test && (((u8 *)p)[0x341] == 0)) && (damage > 0))
            (*((u32 *)(((u8 *)p) + 0x3a8)))++;
    }
    if ((*((u16 *)(((u8 *)p) + 0x350))) == 0) {
        if (((*((u16 *)(((u8 *)p) + 0x34e))) == 0) && SceneActorGraphics_GetEffectState(p)) {
            SceneActors_SpawnDestructionParticles(p);
            damage = (*((s16 *)(((u8 *)gUnk_02000710) + 0x876))) * 5;
            Sound_PlaySongChecked(0x12a);
            *((u16 *)(((u8 *)p) + 0x350)) = (t1 = 60);
        }
    } else {
        *((u16 *)(((u8 *)p) + 0x350)) -= 1;
    }
    if ((damage > 0) || (((u8 *)p)[0x341] != 0)) {
        {
            s16 *h = &base->health;
            *h -= damage;
            hp = h;
        }
        if (((u8 *)p)[0x341]) {
            trigger = 1;
        } else if ((*((u32 *)(((u8 *)p) + 0x3a8))) >= ((u32)(*((s16 *)(((u8 *)p) + 0x376))))) {
            trigger = 1;
            *((u32 *)(((u8 *)p) + 0x3a8)) = 0;
        } else {
            trigger = 0;
        }
        *((u16 *)(((u8 *)p) + 0x34c)) = 10;
        if (special) {
            *((void **)(((u8 *)p) + 0x288)) = ((u8 *)p) + 0x7c4;
            SceneActors_SpawnDestructionParticles(p);
        } else {
            *((void **)(((u8 *)p) + 0x288)) = ((u8 *)p) + 0x5c4;
        }
        size.x = 16;
        size.y = 0;
        size.z = 16;
        DamageNumberDisplay_Show(collider, damage, 0, sprite + 0xc, &size, 1);
        if ((*hp) <= 0) {
            {
                u8 k1 = 0;
                *hp = k1;
            }
            {
                u32 mask = 2;
                *flags |= mask;
            }
            Vampire_SetState(base, sub_08027214, 25);
            return 1;
        }
        if (((u8 *)p)[0x341] == 0) {
            if (special) {
                u32 mask = 0x80000;
                *flags |= mask;
            }
            Sound_PlaySongChecked(0x13e);
        }
        if ((*((s32 *)(((u8 *)p) + 0x358))) == 3)
            trigger = flaggedBit;
        if (trigger) {
            {
                u32 mask = 1;
                *flags |= mask;
            }
            Vampire_SetState_1(base, sub_08026FF0);
            return 1;
        }
        ((u8 *)p)[0x399] = t1 - 52;
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 index = gUnk_03005308 * 2;
            u32 value;
            value = *((u16 *)(((u8 *)0x0203B400) + index));
            ((u8 *)p)[0x398] = value;
        }
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", Vampire_ApplyPendingDamage);
#endif
