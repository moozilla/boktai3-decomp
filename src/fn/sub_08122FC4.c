#include "global.h"
#define SpriteNotice_Create sub_08122FC4
#define SpriteNotice_GetManager SceneTrailEffect_EnsurePool
#define SpriteNotice_AllocateSlot sub_08122F78
#define gSpriteNoticeManager gUnk_02000144
struct SpriteNoticePosition { s16 x,y,z; };
struct SpriteNotice { u32 flags; u8 unk4[11]; u8 mode; u8 unk10[8]; struct SpriteNoticePosition position; u8 unk20[8]; u8 animation[0x1A]; u8 activeX, unk43, activeY; };
struct SpriteNoticeManager { u8 task[0x18]; void *animationData, *spriteData; };
extern struct SpriteNoticeManager *gSpriteNoticeManager;
struct SpriteNoticeManager *SpriteNotice_GetManager(void);
struct SpriteNotice *SpriteNotice_AllocateSlot(struct SpriteNoticeManager *);
void sub_08217DE0(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
u32 Anim_SetSequence(void *,void *,u32,u32,u32);
s32 sub_08122FC4(struct SpriteNoticePosition *position, u32 mode)
{
    struct SpriteNoticeManager *manager;
    struct SpriteNotice *slot;
    if (!position) return -1;
    manager=gSpriteNoticeManager;
    if (!manager) {
        manager=SpriteNotice_GetManager();
        if (!manager) return -1;
    }
    slot=SpriteNotice_AllocateSlot(manager);
    if (!slot) return -1;
    slot->position=*position;
    sub_08217DE0(slot,manager->spriteData,0);
    Effect_SetOrigin(slot,-8,-8);
    { u32 zero=0; Anim_SetSequence(slot->animation,manager->animationData,zero,0,zero); }
    slot->flags |= 1;
    slot->mode=mode;
    slot->activeX=1;
    slot->activeY=1;
    return 0;
}
