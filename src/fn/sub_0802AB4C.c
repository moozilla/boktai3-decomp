#include "global.h"
struct Common {
    u8 pad0[0x90];
    u32 timer;
    s16 health, maxHealth;
    u8 pad98[4];
    u32 flags;
    u8 padA0[2];
    u8 reset;
    u8 padA3;
    u32 pending;
    u8 facing, facing8, requested, state;
    u8 active, actionReset, action, finished, entered, changed;
    u8 padB2[0x12];
    u32 actionTimer;
    u8 padC8[0x38];
    void (*callback)(void *);
};
static inline void state(struct Common *p, void (*fn)(void *), int id)
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
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void sub_080472A4(void *, int, int, void *, void *, int);
void sub_0802A4F4(void *);
void sub_0802A53C(void *);
void sub_0802A63C(void *);
struct Actor {
    struct Common common;
    u8 pad104[0x74];
    int damage;
    u8 special;
    u8 pad17d[15];
    s16 flash, stun;
    u8 pad190[4];
    u8 angle, shake, pad196[2];
    int accumulated, threshold;
};
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void sub_0802A5A0(void *);
void Sound_PlaySongChecked(int);
int sub_0802AB4C(struct Actor *p)
{
    struct Common *base = (void *)p;
    u8 *position = (u8 *)p + 0x48;
    u8 *collider = (u8 *)p + 0x818;
    int damage, special;
    struct Packed size;
    {
        int offset = 0x178;
        int *damagePtr = (int *)((u8 *)p + offset);
        damage = *damagePtr;
        {
            int off = 0x17c;
            u8 *specialPtr = (u8 *)p + off;
            int value;
            value = *specialPtr;
            special = value;
            *damagePtr = 0;
            *specialPtr = 0;
        }
    }
    if (damage > 0) {
        int react = 0;
        p->common.health -= damage;
        {
            int offset = 0x198;
            int *accum = (int *)((u8 *)p + offset);
            {
                int value = *accum + damage;
                *accum = value;
                {
                    int off = 0x19c;
                    int *threshold = (int *)((u8 *)p + off);
                    if (value >= *threshold) {
                        value = react;
                        *accum = value;
                        {
                            int one = 1;
                            react = one;
                        }
                    }
                }
            }
        }
        p->stun = 15;
        p->flash = 10;
        if (special)
            *(void **)((u8 *)p + 0x878) = (u8 *)p + 0x5c4;
        else
            *(void **)((u8 *)p + 0x878) = (u8 *)p + 0x3c4;
        size.x = 16;
        size.y = 0;
        size.z = 16;
        sub_080472A4(collider, damage, 0, position + 12, &size, 1);
        if (base->health <= 0) {
            base->health = 0;
            {
                u32 mask = 2;
                base->flags |= mask;
            }
            state(base, sub_0802A63C, 8);
            return 1;
        }
        if (special) {
            u32 mask = 0x80000;
            base->flags |= mask;
        }
        if (react) {
            {
                u32 mask = 1;
                base->flags |= mask;
            }
            state(base, sub_0802A5A0, 7);
            return 1;
        }
        p->shake = 3;
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 index = gUnk_03005308 * 2;
            u32 value;
            value = *(u16 *)((u8 *)((u16 *)RANDOM_TABLE_ADDR) + index);
            p->angle = value;
        }
        Sound_PlaySongChecked(318);
    }
    return 0;
}
