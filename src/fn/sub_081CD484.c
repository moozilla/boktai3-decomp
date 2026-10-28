// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
#include "player.h"
typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    s16 *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;

struct Vec {
    s16 x, y, z, w;
};

struct Sub {
    u8 pad0[4];
    u16 orb;
    s16 tag;
    u8 pad8[8];
    s16 fadeMode, fadeCounter;
    u32 bankB, bankA;
};

struct Motion {
    u8 pad[9];
    u8 angle;
    u8 padA[2];
    struct Vec pos;
    u8 pad14[0x20];
    u16 x, y, z;
};
struct SceneNpc {
    u8 pad0;
    u8 kind;
    u8 state, entered, sub;
    u8 pad5[3];
    u8 changed;
    u8 pad9[3];
    u16 flags;
    u8 padE[0x14 - 0xE];
    u32 timer;
    u8 pad18;
    u8 facing4, facing8, angle;
    u8 pad1C[0x20 - 0x1C];
    struct Sub sub20;
    u8 pad3C[0x54 - 0x3C];
    struct Vec target;
    struct Motion motion;
    u8 pad98[0xEC - 0x98];
    u8 *palette;
    u8 padF0[0x168 - 0xF0];
    Entry entry;
    u8 pad1A0[0x1CC - 0x168 - sizeof(Entry)];
    u8 paletteBuf[0x208 - 0x1CC];
    s32 speed;
    s32 speed2;
};

extern u8 *gSceneEventDispatchState;

void SceneMessages_SetPendingTrigger(void *, void *);
s32 Math_Atan2(s32, s32);

void Actor_SetAttachmentEffect(struct SceneNpc *, s32, s32, s32);

void CpuSet(const void *, void *, u32);

static inline void setState(struct SceneNpc *npc, u32 sub)
{
    npc->state = 0;
    npc->entered = 0;
    npc->sub = sub;
    npc->timer = 0;
}

static inline void setFacing(struct SceneNpc *npc, u32 a)
{
    npc->motion.angle = a;
    {
        s32 t = (s32)(a + 0x70) >> 5;
        u32 m = 7;
        m &= t;
        npc->facing8 = m;
    }
    {
        s32 t = (s32)(a + 0x60) >> 6;
        u32 m = 3;
        m &= t;
        npc->facing4 = m;
    }
}

static inline u32 setInitialFacing(struct SceneNpc *npc, u32 input)
{
    u32 angle = input;
    register u8 *dest asm("r2");
    register u32 zero asm("r3");
    asm("" : "+r"(angle));
    dest = &npc->motion.angle;
    zero = 0;
    asm("" : : "r"(zero));
    *dest = angle;
    {
        s32 t = (s32)(angle + 0x70) >> 5;
        u32 m = 7;
        m &= t;
        npc->facing8 = m;
    }
    {
        s32 t = (s32)(angle + 0x60) >> 6;
        u32 m = 3;
        m &= t;
        npc->facing4 = m;
    }
    return zero;
}
static inline void setInitialState(struct SceneNpc *npc, u32 state, u32 zero)
{
    npc->state = zero;
    npc->entered = zero;
    npc->sub = state;
    npc->timer = zero;
}

static inline void aim(struct SceneNpc *npc, s16 *m)
{
    struct Vec *t, *p;
    npc->target.x = m[4];
    npc->target.y = m[5];
    npc->target.z = m[6];
    t = &npc->target;
    p = &npc->motion.pos;
    npc->angle = Math_Atan2(t->x - p->x, t->z - p->z);
}

static inline void setSpeed(struct SceneNpc *npc, s32 speed)
{
    npc->speed = speed;
    npc->speed2 = speed * speed;
}

struct PackedVec {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
struct Camera {
    u8 pad[0x30];
    struct PackedVec position;
};
void *memset(void *, s32, u32);
void *SceneMessages_FindScript(s32);
void Trigger_CreateDrop(void *, s32, s32, void *, void *, s32, s32, s32);
void FloatingItemEffect_Spawn(s32, s32, s32, s32, void *, void *, s32, void *);
void SunOrb_GetOrCreateManager(void);
s32 SunOrb_CreateSceneTargeted(struct PackedVec *);
void Trigger_ConsumePending(void *, s32);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
void SceneNpc_DispatchCutsceneCommands(struct SceneNpc *npc)
{
    Entry *entry = &npc->entry;
    s32 i = 0;
    if (i < entry->count[gSceneEventDispatchState[0x24]])
        do {
            u8 *controller = gSceneEventDispatchState;
            s16 *m = entry->messages[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(entry, m);
            switch (((u8 *)m)[6]) {
                case 0: {
                    setState(npc, 0);
                    npc->changed = 1;
                } break;
                case 1: {
                    u32 zero = setInitialFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    if (npc->kind == 29) {
                        if (npc->sub20.tag == 1) {
                            setInitialState(npc, 93, zero);
                            npc->changed = 1;
                        } else {
                            setInitialState(npc, 1, zero);
                            npc->changed = 1;
                        }
                    } else {
                        setInitialState(npc, 1, zero);
                        npc->changed = 1;
                    }
                } break;
                case 2: {
                    setState(npc, 1);
                    npc->changed = 1;
                } break;
                case 68: {
                    s32 data[3];
                    memset(data, 0, 12);
                    if (m[4] == 0)
                        Trigger_CreateDrop(&npc->motion.pos, m[5], m[6], 0, 0, 0, m[6], 0);
                    else {
                        void *script = SceneMessages_FindScript(m[4]);
                        Trigger_CreateDrop(&npc->motion.pos, m[5], m[6], script, data, 0, m[6], 0);
                    }
                } break;
                case 69: {
                    struct PackedVec pos;
                    s32 a = m[4], b = m[5];
                    struct Vec *p = &npc->motion.pos;
                    pos = (*(struct Camera **)gPlayers)->position;
                    pos.y += 200;
                    FloatingItemEffect_Spawn(a, b, 3, 0, &pos, p, 24, p);
                } break;
                case 4: {
                    setSpeed(npc, 10);
                    aim(npc, m);
                    setState(npc, 16);
                    npc->changed = 1;
                } break;
                case 5: {
                    setSpeed(npc, 20);
                    aim(npc, m);
                    setState(npc, 16);
                    npc->changed = 1;
                } break;
                case 36: {
                    setSpeed(npc, 10);
                    aim(npc, m);
                    setState(npc, 2);
                    npc->changed = 1;
                } break;
                case 3: {
                    npc->angle = (((u8 *)m)[8] + 5) << 5;
                    setState(npc, 3);
                    npc->changed = 1;
                } break;
                case 6: {
                    setState(npc, 4);
                    npc->changed = 1;
                } break;
                case 7: {
                    setState(npc, 5);
                    npc->changed = 1;
                } break;
                case 25: {
                    Actor_SetAttachmentEffect(npc, m[4], 180, 180);
                } break;
                case 65: {
                    setState(npc, 20);
                    npc->changed = 1;
                } break;
                case 66: {
                    setState(npc, 21);
                    npc->changed = 1;
                } break;
                case 67: {
                    setState(npc, 22);
                    npc->changed = 1;
                } break;
                case 64: {
                    npc->motion.pos.x = m[4];
                    npc->motion.pos.y = m[5];
                    npc->motion.pos.z = m[6];
                } break;
                case 62: {
                    setState(npc, 29);
                    npc->changed = 1;
                } break;
                case 59: {
                    setState(npc, 26);
                    npc->changed = 1;
                } break;
                case 60: {
                    setState(npc, 27);
                    npc->changed = 1;
                } break;
                case 61: {
                    setState(npc, 28);
                    npc->changed = 1;
                } break;
                case 29: {
                    setState(npc, 32);
                    npc->changed = 1;
                } break;
                case 34: {
                    setState(npc, 35);
                    npc->changed = 1;
                } break;
                case 35: {
                    setState(npc, 36);
                    npc->changed = 1;
                } break;
                case 32: {
                    setState(npc, 40);
                    npc->changed = 1;
                } break;
                case 33: {
                    setState(npc, 41);
                    npc->changed = 1;
                } break;
                case 39: {
                    setState(npc, 42);
                    npc->changed = 1;
                } break;
                case 52: {
                    setState(npc, 44);
                    npc->changed = 1;
                } break;
                case 38: {
                    setState(npc, 45);
                    npc->changed = 1;
                } break;
                case 49: {
                    setState(npc, 47);
                    npc->changed = 1;
                } break;
                case 50: {
                    setState(npc, 48);
                    npc->changed = 1;
                } break;
                case 51: {
                    setState(npc, 49);
                    npc->changed = 1;
                } break;
                case 53: {
                    setState(npc, 50);
                    npc->changed = 1;
                } break;
                case 54: {
                    setState(npc, 51);
                    npc->changed = 1;
                } break;
                case 55: {
                    setState(npc, 52);
                    npc->changed = 1;
                } break;
                case 28: {
                    setState(npc, 54);
                    npc->changed = 1;
                } break;
                case 70: {
                    setState(npc, 55);
                    npc->changed = 1;
                } break;
                case 71: {
                    setState(npc, 56);
                    npc->changed = 1;
                } break;
                case 72: {
                    aim(npc, m);
                    setSpeed(npc, 10);
                    setState(npc, 57);
                    npc->changed = 1;
                } break;
                case 73: {
                    setState(npc, 58);
                    npc->changed = 1;
                } break;
                case 75: {
                    npc->angle = (((u8 *)m)[8] + 5) << 5;
                    setFacing(npc, (npc->angle + 128) & 255);
                    setSpeed(npc, m[5]);
                    setState(npc, 60);
                    npc->changed = 1;
                } break;
                case 76: {
                    setState(npc, 61);
                    npc->changed = 1;
                } break;
                case 77: {
                    npc->motion.pos.x = m[4];
                    npc->motion.pos.y = m[5];
                    npc->motion.pos.z = m[6];
                    setSpeed(npc, 10);
                    setState(npc, 62);
                    npc->changed = 1;
                } break;
                case 78: {
                    npc->motion.pos.x = m[4];
                    npc->motion.pos.y = m[5];
                    npc->motion.pos.z = m[6];
                    setSpeed(npc, 10);
                    setState(npc, 63);
                    npc->changed = 1;
                } break;
                case 79: {
                    npc->motion.pos.x = m[4];
                    npc->motion.pos.y = m[5];
                    npc->motion.pos.z = m[6];
                    setState(npc, 64);
                    npc->changed = 1;
                } break;
                case 80: {
                    setState(npc, 65);
                    npc->changed = 1;
                } break;
                case 81: {
                    setState(npc, 66);
                    npc->changed = 1;
                } break;
                case 82: {
                    setState(npc, 67);
                    npc->changed = 1;
                } break;
                case 84: {
                    setState(npc, 70);
                    npc->changed = 1;
                } break;
                case 85: {
                    setState(npc, 71);
                    npc->changed = 1;
                } break;
                case 86: {
                    setState(npc, 72);
                    npc->changed = 1;
                } break;
                case 87: {
                    setState(npc, 73);
                    npc->changed = 1;
                } break;
                case 88: {
                    setState(npc, 74);
                    npc->changed = 1;
                } break;
                case 89: {
                    setState(npc, 75);
                    npc->changed = 1;
                } break;
                case 90: {
                    setState(npc, 76);
                    npc->changed = 1;
                } break;
                case 91: {
                    setState(npc, 77);
                    npc->changed = 1;
                } break;
                case 92: {
                    setState(npc, 79);
                    npc->changed = 1;
                } break;
                case 93: {
                    u32 zero = setInitialFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    setInitialState(npc, 83, zero);
                    npc->changed = 1;
                } break;
                case 94: {
                    setState(npc, 84);
                    npc->changed = 1;
                } break;
                case 95: {
                    setState(npc, 85);
                    npc->changed = 1;
                } break;
                case 96: {
                    u32 zero = setInitialFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    setInitialState(npc, 86, zero);
                    npc->changed = 1;
                } break;
                case 97: {
                    setState(npc, 89);
                    npc->changed = 1;
                } break;
                case 98: {
                    setState(npc, 90);
                    npc->changed = 1;
                } break;
                case 99: {
                    setSpeed(npc, m[7]);
                    aim(npc, m);
                    setState(npc, 91);
                    npc->changed = 1;
                } break;
                case 100: {
                    struct Sub *sub = &npc->sub20;
                    struct PackedVec pos;
                    pos.x = (u16)m[4];
                    pos.y = (u16)m[5];
                    pos.z = (u16)m[6];
                    SunOrb_GetOrCreateManager();
                    sub->orb = SunOrb_CreateSceneTargeted(&pos);
                    Trigger_ConsumePending(&npc->entry, 1);
                } break;
                case 101: {
                    setState(npc, 92);
                    npc->changed = 1;
                } break;
                case 102: {
                    setState(npc, 19);
                    npc->changed = 1;
                } break;
                case 103: {
                    npc->sub20.tag = m[4];
                    if ((s16)npc->sub20.tag == 1) {
                        setState(npc, 93);
                        npc->changed = 1;
                        {
                            struct Motion *motion = &npc->motion;
                            s32 speed = 528;
                            motion->x = 0;
                            motion->y = speed;
                            motion->z = 0;
                        }
                    } else {
                        {
                            struct Motion *motion = &npc->motion;
                            s32 speed = 400;
                            motion->x = 0;
                            motion->y = speed;
                            motion->z = 0;
                        }
                    }
                    Trigger_ConsumePending(&npc->entry, 1);
                } break;
                case 104: {
                    struct Sub *sub = &npc->sub20;
                    s32 bank;
                    sub->fadeCounter = 0;
                    sub->fadeMode = m[4];
                    if (m[5] == 1)
                        bank = 0x174;
                    else if (m[5] == 9)
                        bank = 0x16f;
                    else
                        bank = 0x34;
                    sub->bankA = bank;
                    if (sub->fadeMode == 0) {
                        u8 *p = gPaletteBankData + (bank << 5);
                        Palette_InterpolateBanks(npc->paletteBuf, p, p, 2, 1);
                        sub->bankB = sub->bankA;
                    }
                    Trigger_ConsumePending(&npc->entry, 1);
                } break;
                case 105: {
                    setState(npc, 94);
                    npc->changed = 1;
                } break;
                case 106: {
                    setState(npc, 95);
                    npc->changed = 1;
                } break;
                case 107: {
                    setState(npc, 96);
                    npc->changed = 1;
                } break;
                case 30: {
                    if (npc->kind == 9) {
                        setState(npc, 25);
                        npc->changed = 1;
                    } else if (npc->kind == 4) {
                        setState(npc, 33);
                        npc->changed = 1;
                    }
                } break;
                case 27: {
                    if (npc->kind == 5) {
                        setState(npc, 38);
                        npc->changed = 1;
                    } else if (npc->kind == 4) {
                        setState(npc, 31);
                        npc->changed = 1;
                    } else if (npc->kind == 27) {
                        setState(npc, 53);
                        npc->changed = 1;
                    } else if ((u8)(npc->kind - 9) <= 1) {
                        setState(npc, 24);
                        npc->changed = 1;
                    } else if (npc->kind == 29) {
                        setState(npc, 88);
                        npc->changed = 1;
                    }
                } break;
                case 26: {
                    if ((u8)(npc->kind - 9) <= 1) {
                        setState(npc, 23);
                        npc->changed = 1;
                    } else if (npc->kind == 4) {
                        setState(npc, 30);
                        npc->changed = 1;
                    } else if (npc->kind == 5) {
                        setState(npc, 37);
                        npc->changed = 1;
                    } else if (npc->kind == 27) {
                        setState(npc, 69);
                        npc->changed = 1;
                    } else if (npc->kind == 28) {
                        setState(npc, 80);
                        npc->changed = 1;
                    } else if (npc->kind == 29) {
                        setState(npc, 87);
                        npc->changed = 1;
                    }
                } break;
                case 83: {
                    if (npc->kind == 27) {
                        setState(npc, 68);
                        npc->changed = 1;
                    } else if (npc->kind == 28) {
                        setState(npc, 81);
                        npc->changed = 1;
                    }
                } break;
                case 31: {
                    if (npc->kind == 4) {
                        setState(npc, 34);
                        npc->changed = 1;
                    } else if (npc->kind == 5) {
                        setState(npc, 39);
                        npc->changed = 1;
                    }
                } break;
                case 40: {
                    if (npc->kind == 5) {
                        setState(npc, 43);
                        npc->changed = 1;
                    } else if (npc->kind == 15) {
                        setState(npc, 46);
                        npc->changed = 1;
                    }
                } break;
                case 74: {
                    npc->angle = (((u8 *)m)[8] + 5) << 5;
                    setFacing(npc, (npc->angle + 128) & 255);
                    setSpeed(npc, m[5]);
                    if (npc->kind == 27) {
                        setState(npc, 59);
                        npc->changed = 1;
                    } else if (npc->kind == 28) {
                        setState(npc, 82);
                        npc->changed = 1;
                    }
                } break;
                case 108: {
                    setState(npc, 99);
                    npc->changed = 1;
                } break;
            }
            i++;
        } while (i < entry->count[gSceneEventDispatchState[0x24]]);
}
