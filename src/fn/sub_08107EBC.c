// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "scene_vec.h"
#include "ram.h"
struct Frames {
    u16 *data;
    u8 flags, count, multiplier, duration;
    u16 index, padA, rate, timer;
};
struct TriggerSprite {
    u32 flags;
    u8 pad4[3];
    u8 priority;
    u8 pad8[8];
    u16 frame;
    u8 pad12[0xA];
    struct SceneVec position;
};
struct TileTrigger {
    struct TriggerSprite sprite;
    u8 pad24[8];
    u8 body[0x48];
    struct Frames frames;
    u8 pad84[0];
    struct SceneVec position;
    s32 key;
    s32 region;
    u16 cost;
    u8 pad96[2];
    s32 params[6];
    s32 args[2];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
};
struct TileTriggerManager {
    u8 unknown0[0x18];
    void *animationData;
    u8 resource[0x1C];
};
struct TileTriggerManager *Pool1B8_Create(void);
struct TileTrigger *Pool1B8_Allocate(struct TileTriggerManager *);
void Pool1B8_EnterStateThree(struct TileTriggerManager *, struct TileTrigger *);
void sub_08107D54(struct TileTriggerManager *, struct TileTrigger *);
void SceneStatusIndicator_Create(struct SceneVec *);
void SceneBody_Init(void *, s32, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
void sub_08107EBC(void)
{
    struct TileTriggerManager *manager = gTileTriggerManager;
    struct TileTrigger *p;
    struct SceneVec pos;
    struct Frames *f;
    const vu16 *frame;
    struct TriggerSprite *sprite;
    if (!manager) {
        manager = Pool1B8_Create();
        if (!manager)
            return;
    }
    p = Pool1B8_Allocate(manager);
    if (!p)
        return;
    p->state = 1;
    if (Script_SeekToKeyword('p')) {
        p->position.x = Script_GetValue();
        p->position.y = Script_GetValue();
        p->position.z = Script_GetValue();
    }
    p->key = Script_GetKeywordValue('i', -1);
    p->region = Script_GetKeywordValue('R', 0);
    if (Script_SeekToKeyword('G')) {
        s32 *dst = p->params;
        s32 i;
        for (i = 5; i >= 0; i--) {
            s32 v;
            if (Script_GetPc())
                v = Script_GetValue();
            else
                v = 0;
            *dst++ = v;
        }
    }
    p->cost = Script_GetKeywordValue('C', 0);
    if (Script_SeekToKeyword('A')) {
        s32 *dst = p->args;
        s32 i;
        for (i = 1; i >= 0; i--) {
            s32 v;
            if (Script_GetPc())
                v = Script_GetValue();
            else
                v = 0;
            *dst++ = v;
        }
    }
    {
        s32 v = Script_GetKeywordValue('T', 0);
        p->sequence = v;
        p->sequence = (u16)v > 4 ? 4 : v;
    }
    sprite = &p->sprite;
    p->sprite.position = p->position;
    p->sprite.position.x -= 0x100;
    switch (p->sequence) {
        case 0:
            AnimData_SelectPaletteBank(manager->resource, 0x2EA);
            break;
        case 1:
            AnimData_SelectPaletteBank(manager->resource, 0x2EB);
            break;
    }
    Sprite_Init(sprite, manager->resource, 0);
    Anim_SetSequence(&p->frames, manager->animationData, 0, *(u8 *)&p->sequence, 0);
    f = &p->frames;
    frame = &f->data[f->index];
    sprite->frame = *frame >> 6;
    {
        u32 input = f->flags;
        u32 one = 1;
        u32 first;
        asm("" : "=r"(first) : "0"(one));
        first &= input;
        if (first != (((*frame & 0x30) >> 4) & one))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 flags = f->flags;
        u32 two = 2;
        u32 first;
        asm("" : "=r"(first) : "0"(two));
        first &= flags;
        asm("" : "+r"(first));
        if ((u8)first != (((*frame & 0x30) >> 4) & two))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++f->timer >= f->duration) {
        f->timer = 0;
        if (f->flags & 4) {
            u32 index = f->index;
            if (index == 0) {
                if (f->flags & 8) {
                    f->timer = f->duration;
                    goto next;
                }
                index = f->count;
            }
            f->index = index - 1;
        } else if (++f->index >= f->count) {
            if ((u8)(f->flags & 8)) {
                f->timer = f->duration;
                f->index = f->count - 1;
            } else
                f->index = 0;
        }
    next:
        frame = &f->data[f->index];
        f->multiplier = *frame & 0xF;
        f->duration = (f->multiplier * f->rate) >> 6;
        if (!f->duration)
            f->duration = 1;
    }
    sprite->flags |= 1;
    sprite->priority = 2;
    p->frame = 0;
    p->forced = 0;
    p->triggerFlags = 1;
    p->delay = Script_GetKeywordValue('o', 60);
    if (Script_GetKeywordValue('m', 0)) {
        p->triggerFlags |= 0x10;
        switch (Script_GetKeywordValue('S', 0)) {
            case 0:
                Pool1B8_EnterStateThree(manager, p);
                break;
            case 1:
                sub_08107D54(manager, p);
                break;
        }
    }
    pos = p->position;
    SceneStatusIndicator_Create(&pos);
    if (Script_GetKeywordValue('s', 0)) {
        p->triggerFlags |= 8;
        SceneBody_Init(p->body, p->key, &p->position, 0, 7, p);
        SceneBody_SetSprite(p->body, p);
        TerrainOverlay_QueueAddActor(p->body);
    }
}
