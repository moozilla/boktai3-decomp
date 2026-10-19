#include "global.h"
#include "scene_hud_display.h"

#ifdef NONMATCHING
// fuzzy: 92.0%
// foundry draft (worker best 90.8%); what differs is mostly register allocation and instruction order
#define SceneHud_InitializeOwner sub_08178AA0
#define gSceneHud                gUnk_02000214
#define gSceneHudOwnerSource     gUnk_020004B4
#define gPartyActors             gUnk_02000580
struct SceneHudOwnerSource {
    u8 unknown0[0x18];
    s16 index;
};
extern struct SceneHudOwnerSource *gSceneHudOwnerSource;
extern struct Player *gPartyActors[];
extern struct SceneHudState *gSceneHud;

void sub_081779E8(struct SceneHudState *);
void SceneHud_InitGameplayResources(struct SceneHudState *);
void SceneHud_RefreshGameplayDisplay(struct SceneHudState *);
static inline s32 OwnerIndex(void)
{
    if (!gSceneHudOwnerSource)
        return -1;
    return gSceneHudOwnerSource->index;
}
s32 sub_08178AA0(struct SceneHudState *s)
{
    s32 index;
    u32 offset;
    struct Player *owner;
    s->initializationFlag = 0;
    Bg_SetPriority(0, 0);
    index = OwnerIndex();
    s->actionOwnerIndex = index;
    offset = 0xA5C;
    *(u16 *)((u8 *)s + offset) = 0;
    owner = gPartyActors[s->actionOwnerIndex];
    s->actionOwner = owner;
    sub_081779E8(s);
    SceneHud_InitGameplayResources(s);
    SceneHud_RefreshGameplayDisplay(s);
    s->previousCueGroup = 255;
    s->previousCueSelection = -1;
    offset = 0xA5F;
    *(s8 *)((u8 *)s + offset) = -1;
    gSceneHud = s;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08178AA0);
#endif
