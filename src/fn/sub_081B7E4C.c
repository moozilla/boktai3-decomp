// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "terrain_cell_probe.h"

void SceneBody_Init(void *, u32, void *, u32, u32, void *);
void TerrainOverlay_QueueAddActor(void *);
void sub_081B7DC8(void *);
void AnimatedMarker_RegisterOccupiedCell(void *);
void GrowthHazard_RegisterCellEffect(void *);
void GrowthHazard_UpdateCooldown(void *);
void GrowthHazard_UpdateIdle(void *);

struct Mgr {
    u8 pad[0x18];
    u8 count;
};

struct Two {
    u32 a, b;
};

void GrowthHazard_InitEntryFromScript(u8 *input)
{
    register struct Mgr *manager asm("r8") = (struct Mgr *)input;
    u8 *entry;
    u8 *sprite;
    register u16 *collisionId asm("r9");
    u16 *world;
    u32 present;
    {
        u32 count = manager->count;
        entry = (u8 *)manager + (count * 0x118 + 0x5C);
    }
    {
        u32 found = Script_SeekToKeyword(0x6E);
        if (found) {
            u32 v = Script_GetValue();
            u16 *p = (u16 *)(entry + 0xEE);
            *p = v;
            collisionId = p;
        } else {
            u16 *p = (u16 *)(entry + 0xEE);
            *p = found;
            collisionId = p;
        }
    }
    present = Script_SeekToKeyword(0x70);
    if (present) {
        u32 v = Script_GetValue();
        u16 *w = (u16 *)(entry + 0xE4);
        *w = v;
        *(u16 *)(entry + 0xE6) = Script_GetValue();
        *(u16 *)(entry + 0xE8) = Script_GetValue();
        world = w;
    } else {
        u16 *w = (u16 *)(entry + 0xE4);
        *w = present;
        *(u16 *)(entry + 0xE6) = present;
        *(u16 *)(entry + 0xE8) = present;
        world = w;
    }
    *(u16 *)(entry + 0xF2) = cellIndexAt((struct Pos16 *)world);
    {
        u32 resource = 0xC3C3;
        void *animation;
        sprite = entry;
        animation = entry + 0x2C;
        AnimData_Load(animation, resource);
        Sprite_Init(sprite, animation, 0);
        sprite[7] = 3;
    }
    if (Script_SeekToKeyword(0x72) && Script_GetValue())
        *(u32 *)entry |= 4;
    *(struct Two *)(sprite + 0x1C) = *(struct Two *)world;
    if (Script_SeekToKeyword(0x68)) {
        entry[0xED] = Script_GetValue();
        if (entry[0xED]) {
            void *body = entry + 0x9C;
            {
                register u16 *cid asm("r2") = collisionId;
                asm("" : "+r"(cid));
                SceneBody_Init(body, *cid, sprite + 0x1C, 0, 7, entry);
            }
            TerrainOverlay_QueueAddActor(body);
        }
    } else
        entry[0xED] = 0;
    *(u16 *)(entry + 0xF8) = 0;
    if (Script_SeekToKeyword(0x41))
        *(u16 *)(entry + 0xFA) = Script_GetValue();
    else
        *(u16 *)(entry + 0xFA) = 3;
    *(u16 *)(entry + 0xFC) = 0;
    if (Script_SeekToKeyword(0x6C))
        *(u16 *)(entry + 0xFE) = Script_GetValue();
    else
        *(u16 *)(entry + 0xFE) = 3;
    sub_081B7DC8(entry);
    {
        register u32 off asm("r3") = 0x100;
        register u8 *p asm("r1");
        asm("" : "+r"(off));
        p = entry + off;
        *p = 0;
    }
    {
        u32 found = Script_SeekToKeyword(0x73);
        register u8 *p asm("r1");
        u32 val = found;
        if (found) {
            val = Script_GetValue();
            {
                register u32 off asm("r2") = 0x102;
                asm("" : "+r"(off));
                p = entry + off;
            }
        } else {
            {
                register u32 off asm("r3") = 0x102;
                asm("" : "+r"(off));
                p = entry + off;
            }
        }
        *p = val;
    }
    {
        u32 flag;
        u32 value;
        register u16 *bob asm("r1");
        {
            register u32 off asm("r1") = 0x102;
            register u8 *q asm("r0");
            asm("" : "+r"(off));
            q = entry + off;
            flag = *q;
        }
        if (flag != 0) {
            u32 two, zero;
            AnimatedMarker_RegisterOccupiedCell(entry);
            two = 2;
            zero = 0;
            *(u16 *)(sprite + 0x10) = two;
            sprite[7] = 2;
            {
                register void *fn asm("r1") = GrowthHazard_UpdateCooldown;
                asm("" : "+r"(fn));
                *(void **)(entry + 0x114) = fn;
            }
            *(u16 *)(entry + 0xF4) = zero;
            {
                u16 *t = (u16 *)(entry + 0x48);
                asm("" : "+r"(t));
                bob = t;
            }
            value = 0x20;
        } else {
            GrowthHazard_RegisterCellEffect(entry);
            entry[0x103] = flag;
            *(u16 *)(sprite + 0x10) = flag;
            sprite[7] = 3;
            {
                register void *fn asm("r1") = GrowthHazard_UpdateIdle;
                asm("" : "+r"(fn));
                *(void **)(entry + 0x114) = fn;
            }
            *(u16 *)(entry + 0xF4) = flag;
            {
                u16 *t = (u16 *)(entry + 0x48);
                asm("" : "+r"(t));
                bob = t;
            }
            value = 0x10;
        }
        bob[5] = value;
    }
    *(u16 *)(entry + 0xF6) = 0;
    {
        register struct Mgr *m asm("r3") = manager;
        m->count++;
    }
}
