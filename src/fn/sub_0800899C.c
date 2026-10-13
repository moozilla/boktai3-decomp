#include "global.h"
struct Vec {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
struct Graphics {
    u32 pad;
    u16 base;
};
struct Sprite {
    u32 flags;
    u8 pad4[4];
    u16 tile;
    u8 width, height;
    u8 padC[3];
    u8 mode, priority;
    u8 pad11[7];
    struct Vec position;
    u8 pad20[8];
};
struct Trigger {
    s16 id, kind;
    s8 actor;
    u8 type, flags, flip, persistent, started, visible, flash, tick, cooldown, armed, shake;
    s16 baseX, baseZ;
    u8 option, condition;
    u16 sound;
    u32 timer;
    struct Sprite *sprite;
    void *object, *other, *animation;
    struct Vec *position;
    u32 startScript, script, arguments[3];
    void (*update)(void *, struct Trigger *);
    u8 effect[108];
    struct Trigger *previous, *next;
};
struct Manager {
    u8 task[24];
    struct Graphics *graphics;
    void *resource;
    u8 pad20[8];
    struct Trigger *head;
};
struct Manager *sub_08008270(void);
void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
void Trigger_UpdateFalling(void *, struct Trigger *);
void sub_08217DE0(struct Sprite *, struct Graphics *, u32);
void sub_08217EC4(struct Sprite *, s32, s32);
s32 sub_0821D6F4(struct Vec *, s32);
s32 sub_0822C034(struct Vec *, s32);
void ScaledShadow_Initialize(void *, struct Vec *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void sub_08006FAC(struct Trigger *);
void sub_080077C4(void *, struct Trigger *);
s32 Trigger_MoveDropsByItem(struct Vec *position, s32 id, s32 kind)
{
    struct Manager *manager = sub_08008270();
    struct Trigger *p, *next;
    if (!manager)
        return -1;
    for (p = manager->head; p; p = next) {
        next = p->next;
        if (p->type == 0 && p->option && p->id == id && p->kind == kind &&
            p->update != sub_080077C4)
            *p->position = *position;
    }
}
