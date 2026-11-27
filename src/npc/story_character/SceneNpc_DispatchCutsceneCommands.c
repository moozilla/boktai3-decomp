#include "npc/story_character/scene_character.h"
#include "scene/motion/scene_facing.h"
#include "engine/video/render.h"
#include "player/player.h"
#include "scene/collision/packed_contact_vector.h"

extern u8 *gSceneEventDispatchState;

void SceneMessages_SetPendingTrigger(void *, void *);
s32 Math_Atan2(s32, s32);

void Actor_SetAttachmentEffect(struct SceneNpc *, s32, s32, s32);

void CpuSet(const void *, void *, u32);

static inline void setState(struct SceneNpc *npc, u32 sub)
{
    npc->stage = 0;
    npc->stageEntered = 0;
    npc->state = sub;
    npc->timer = 0;
}

static inline void setFacing(struct SceneNpc *npc, u32 a)
{
    npc->body.direction = a;
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
    u8 angle = input;
    u8 *dest;
    u32 zero;
    dest = &npc->body.direction;
    zero = 0;
    *dest = angle;
    npc->facing8 = SceneFacing_EightWay(input);
    npc->facing4 = SceneFacing_FourWay(input);
    return zero;
}
static inline void setInitialState(struct SceneNpc *npc, u32 state, u32 zero)
{
    npc->stage = zero;
    npc->stageEntered = zero;
    npc->state = state;
    npc->timer = zero;
}

static inline void aim(struct SceneNpc *npc, s16 *m)
{
    struct ContactPosition *t, *p;
    npc->target.x = m[4];
    npc->target.y = m[5];
    npc->target.z = m[6];
    t = &npc->target;
    p = &npc->body.position;
    npc->targetAngle = Math_Atan2(t->x - p->x, t->z - p->z);
}

static inline void setSpeed(struct SceneNpc *npc, s32 speed)
{
    npc->speed = speed;
    npc->speedSquared = speed * speed;
}

struct Camera {
    u8 pad[0x30];
    struct PackedContactVector position;
};
void *memset(void *, s32, u32);
void *SceneMessages_FindScript(s32);
void Trigger_CreateDrop(void *, s32, s32, void *, void *, s32, s32, s32);
void FloatingItemEffect_Spawn(s32, s32, s32, s32, void *, void *, s32, void *);
void SunOrb_GetOrCreateManager(void);
s32 SunOrb_CreateSceneTargeted(struct PackedContactVector *);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
void SceneNpc_DispatchCutsceneCommands(struct SceneNpc *npc)
{
    struct SceneEventReceiver *receiver = &npc->receiver;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState[0x24]])
        do {
            u8 *controller = gSceneEventDispatchState;
            s16 *m = (s16 *)receiver->pending[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(receiver, m);
            switch (((u8 *)m)[6]) {
                case 0: {
                    setState(npc, 0);
                    npc->commandEntered = 1;
                } break;
                case 1: {
                    u32 zero = setInitialFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    if (npc->kind == 29) {
                        if (npc->effects.tag == 1) {
                            setInitialState(npc, 93, zero);
                            npc->commandEntered = 1;
                        } else {
                            setInitialState(npc, 1, zero);
                            npc->commandEntered = 1;
                        }
                    } else {
                        setInitialState(npc, 1, zero);
                        npc->commandEntered = 1;
                    }
                } break;
                case 2: {
                    setState(npc, 1);
                    npc->commandEntered = 1;
                } break;
                case 68: {
                    s32 data[3];
                    memset(data, 0, 12);
                    if (m[4] == 0)
                        Trigger_CreateDrop(&npc->body.position, m[5], m[6], 0, 0, 0, m[6], 0);
                    else {
                        void *script = SceneMessages_FindScript(m[4]);
                        Trigger_CreateDrop(&npc->body.position, m[5], m[6], script, data, 0, m[6],
                                           0);
                    }
                } break;
                case 69: {
                    struct PackedContactVector pos;
                    s32 a = m[4], b = m[5];
                    struct ContactPosition *p = &npc->body.position;
                    pos = (*(struct Camera **)gPlayers)->position;
                    pos.y += 200;
                    FloatingItemEffect_Spawn(a, b, 3, 0, &pos, p, 24, p);
                } break;
                case 4: {
                    setSpeed(npc, 10);
                    aim(npc, m);
                    setState(npc, 16);
                    npc->commandEntered = 1;
                } break;
                case 5: {
                    setSpeed(npc, 20);
                    aim(npc, m);
                    setState(npc, 16);
                    npc->commandEntered = 1;
                } break;
                case 36: {
                    setSpeed(npc, 10);
                    aim(npc, m);
                    setState(npc, 2);
                    npc->commandEntered = 1;
                } break;
                case 3: {
                    npc->targetAngle = (((u8 *)m)[8] + 5) << 5;
                    setState(npc, 3);
                    npc->commandEntered = 1;
                } break;
                case 6: {
                    setState(npc, 4);
                    npc->commandEntered = 1;
                } break;
                case 7: {
                    setState(npc, 5);
                    npc->commandEntered = 1;
                } break;
                case 25: {
                    Actor_SetAttachmentEffect(npc, m[4], 180, 180);
                } break;
                case 65: {
                    setState(npc, 20);
                    npc->commandEntered = 1;
                } break;
                case 66: {
                    setState(npc, 21);
                    npc->commandEntered = 1;
                } break;
                case 67: {
                    setState(npc, 22);
                    npc->commandEntered = 1;
                } break;
                case 64: {
                    npc->body.position.x = m[4];
                    npc->body.position.y = m[5];
                    npc->body.position.z = m[6];
                } break;
                case 62: {
                    setState(npc, 29);
                    npc->commandEntered = 1;
                } break;
                case 59: {
                    setState(npc, 26);
                    npc->commandEntered = 1;
                } break;
                case 60: {
                    setState(npc, 27);
                    npc->commandEntered = 1;
                } break;
                case 61: {
                    setState(npc, 28);
                    npc->commandEntered = 1;
                } break;
                case 29: {
                    setState(npc, 32);
                    npc->commandEntered = 1;
                } break;
                case 34: {
                    setState(npc, 35);
                    npc->commandEntered = 1;
                } break;
                case 35: {
                    setState(npc, 36);
                    npc->commandEntered = 1;
                } break;
                case 32: {
                    setState(npc, 40);
                    npc->commandEntered = 1;
                } break;
                case 33: {
                    setState(npc, 41);
                    npc->commandEntered = 1;
                } break;
                case 39: {
                    setState(npc, 42);
                    npc->commandEntered = 1;
                } break;
                case 52: {
                    setState(npc, 44);
                    npc->commandEntered = 1;
                } break;
                case 38: {
                    setState(npc, 45);
                    npc->commandEntered = 1;
                } break;
                case 49: {
                    setState(npc, 47);
                    npc->commandEntered = 1;
                } break;
                case 50: {
                    setState(npc, 48);
                    npc->commandEntered = 1;
                } break;
                case 51: {
                    setState(npc, 49);
                    npc->commandEntered = 1;
                } break;
                case 53: {
                    setState(npc, 50);
                    npc->commandEntered = 1;
                } break;
                case 54: {
                    setState(npc, 51);
                    npc->commandEntered = 1;
                } break;
                case 55: {
                    setState(npc, 52);
                    npc->commandEntered = 1;
                } break;
                case 28: {
                    setState(npc, 54);
                    npc->commandEntered = 1;
                } break;
                case 70: {
                    setState(npc, 55);
                    npc->commandEntered = 1;
                } break;
                case 71: {
                    setState(npc, 56);
                    npc->commandEntered = 1;
                } break;
                case 72: {
                    aim(npc, m);
                    setSpeed(npc, 10);
                    setState(npc, 57);
                    npc->commandEntered = 1;
                } break;
                case 73: {
                    setState(npc, 58);
                    npc->commandEntered = 1;
                } break;
                case 75: {
                    npc->targetAngle = (((u8 *)m)[8] + 5) << 5;
                    setFacing(npc, (npc->targetAngle + 128) & 255);
                    setSpeed(npc, m[5]);
                    setState(npc, 60);
                    npc->commandEntered = 1;
                } break;
                case 76: {
                    setState(npc, 61);
                    npc->commandEntered = 1;
                } break;
                case 77: {
                    npc->body.position.x = m[4];
                    npc->body.position.y = m[5];
                    npc->body.position.z = m[6];
                    setSpeed(npc, 10);
                    setState(npc, 62);
                    npc->commandEntered = 1;
                } break;
                case 78: {
                    npc->body.position.x = m[4];
                    npc->body.position.y = m[5];
                    npc->body.position.z = m[6];
                    setSpeed(npc, 10);
                    setState(npc, 63);
                    npc->commandEntered = 1;
                } break;
                case 79: {
                    npc->body.position.x = m[4];
                    npc->body.position.y = m[5];
                    npc->body.position.z = m[6];
                    setState(npc, 64);
                    npc->commandEntered = 1;
                } break;
                case 80: {
                    setState(npc, 65);
                    npc->commandEntered = 1;
                } break;
                case 81: {
                    setState(npc, 66);
                    npc->commandEntered = 1;
                } break;
                case 82: {
                    setState(npc, 67);
                    npc->commandEntered = 1;
                } break;
                case 84: {
                    setState(npc, 70);
                    npc->commandEntered = 1;
                } break;
                case 85: {
                    setState(npc, 71);
                    npc->commandEntered = 1;
                } break;
                case 86: {
                    setState(npc, 72);
                    npc->commandEntered = 1;
                } break;
                case 87: {
                    setState(npc, 73);
                    npc->commandEntered = 1;
                } break;
                case 88: {
                    setState(npc, 74);
                    npc->commandEntered = 1;
                } break;
                case 89: {
                    setState(npc, 75);
                    npc->commandEntered = 1;
                } break;
                case 90: {
                    setState(npc, 76);
                    npc->commandEntered = 1;
                } break;
                case 91: {
                    setState(npc, 77);
                    npc->commandEntered = 1;
                } break;
                case 92: {
                    setState(npc, 79);
                    npc->commandEntered = 1;
                } break;
                case 93: {
                    u32 zero = setInitialFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    setInitialState(npc, 83, zero);
                    npc->commandEntered = 1;
                } break;
                case 94: {
                    setState(npc, 84);
                    npc->commandEntered = 1;
                } break;
                case 95: {
                    setState(npc, 85);
                    npc->commandEntered = 1;
                } break;
                case 96: {
                    u32 zero = setInitialFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    setInitialState(npc, 86, zero);
                    npc->commandEntered = 1;
                } break;
                case 97: {
                    setState(npc, 89);
                    npc->commandEntered = 1;
                } break;
                case 98: {
                    setState(npc, 90);
                    npc->commandEntered = 1;
                } break;
                case 99: {
                    setSpeed(npc, m[7]);
                    aim(npc, m);
                    setState(npc, 91);
                    npc->commandEntered = 1;
                } break;
                case 100: {
                    struct SceneCharacterEffects *sub = &npc->effects;
                    struct PackedContactVector pos;
                    pos.x = (u16)m[4];
                    pos.y = (u16)m[5];
                    pos.z = (u16)m[6];
                    SunOrb_GetOrCreateManager();
                    sub->orbId = SunOrb_CreateSceneTargeted(&pos);
                    Trigger_ConsumePending(&npc->receiver, 1);
                } break;
                case 101: {
                    setState(npc, 92);
                    npc->commandEntered = 1;
                } break;
                case 102: {
                    setState(npc, 19);
                    npc->commandEntered = 1;
                } break;
                case 103: {
                    npc->effects.tag = m[4];
                    if ((s16)npc->effects.tag == 1) {
                        setState(npc, 93);
                        npc->commandEntered = 1;
                        {
                            struct SceneActorBody *body = &npc->body;
                            s32 speed = 528;
                            body->positionOffset.x = 0;
                            body->positionOffset.y = speed;
                            body->positionOffset.z = 0;
                        }
                    } else {
                        {
                            struct SceneActorBody *body = &npc->body;
                            s32 speed = 400;
                            body->positionOffset.x = 0;
                            body->positionOffset.y = speed;
                            body->positionOffset.z = 0;
                        }
                    }
                    Trigger_ConsumePending(&npc->receiver, 1);
                } break;
                case 104: {
                    struct SceneCharacterEffects *sub = &npc->effects;
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
                        Palette_InterpolateBanks(npc->workPalette, p, p, 2, 1);
                        sub->bankB = sub->bankA;
                    }
                    Trigger_ConsumePending(&npc->receiver, 1);
                } break;
                case 105: {
                    setState(npc, 94);
                    npc->commandEntered = 1;
                } break;
                case 106: {
                    setState(npc, 95);
                    npc->commandEntered = 1;
                } break;
                case 107: {
                    setState(npc, 96);
                    npc->commandEntered = 1;
                } break;
                case 30: {
                    if (npc->kind == 9) {
                        setState(npc, 25);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 4) {
                        setState(npc, 33);
                        npc->commandEntered = 1;
                    }
                } break;
                case 27: {
                    if (npc->kind == 5) {
                        setState(npc, 38);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 4) {
                        setState(npc, 31);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 27) {
                        setState(npc, 53);
                        npc->commandEntered = 1;
                    } else if ((u8)(npc->kind - 9) <= 1) {
                        setState(npc, 24);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 29) {
                        setState(npc, 88);
                        npc->commandEntered = 1;
                    }
                } break;
                case 26: {
                    if ((u8)(npc->kind - 9) <= 1) {
                        setState(npc, 23);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 4) {
                        setState(npc, 30);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 5) {
                        setState(npc, 37);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 27) {
                        setState(npc, 69);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 28) {
                        setState(npc, 80);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 29) {
                        setState(npc, 87);
                        npc->commandEntered = 1;
                    }
                } break;
                case 83: {
                    if (npc->kind == 27) {
                        setState(npc, 68);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 28) {
                        setState(npc, 81);
                        npc->commandEntered = 1;
                    }
                } break;
                case 31: {
                    if (npc->kind == 4) {
                        setState(npc, 34);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 5) {
                        setState(npc, 39);
                        npc->commandEntered = 1;
                    }
                } break;
                case 40: {
                    if (npc->kind == 5) {
                        setState(npc, 43);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 15) {
                        setState(npc, 46);
                        npc->commandEntered = 1;
                    }
                } break;
                case 74: {
                    npc->targetAngle = (((u8 *)m)[8] + 5) << 5;
                    setFacing(npc, (npc->targetAngle + 128) & 255);
                    setSpeed(npc, m[5]);
                    if (npc->kind == 27) {
                        setState(npc, 59);
                        npc->commandEntered = 1;
                    } else if (npc->kind == 28) {
                        setState(npc, 82);
                        npc->commandEntered = 1;
                    }
                } break;
                case 108: {
                    setState(npc, 99);
                    npc->commandEntered = 1;
                } break;
            }
            i++;
        } while (i < receiver->count[gSceneEventDispatchState[0x24]]);
}
