#include "global.h"
#include "scene_hud_display.h"
#include "ram.h"

#define gPartyActors gUnk_02000580
struct SceneHudOwnerSource {
    u8 unknown0[0x18];
    s16 index;
};
extern struct SceneHudOwnerSource *gLinkSession;
extern struct Player *gPartyActors[];

void Hud_LoadLayoutResources(struct SceneHudState *);
void SceneHud_InitGameplayResources(struct SceneHudState *);
void SceneHud_RefreshGameplayDisplay(struct SceneHudState *);
static inline s32 OwnerIndex(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->index;
}
s32 SceneHud_InitializeOwner(struct SceneHudState *s)
{
    s32 index;
    u32 offset;
    struct Player *owner;
    int one;
    s->initializationFlag = 0;
    Bg_SetPriority(0, 0);
    index = OwnerIndex();
    s->actionOwnerIndex = index;
    offset = 0xA5C;
    *(u16 *)((u8 *)s + offset) = 0;
    {
        u8 idx = s->actionOwnerIndex;
        owner = gPartyActors[idx];
    }
    s->actionOwner = owner;
    Hud_LoadLayoutResources(s);
    SceneHud_InitGameplayResources(s);
    SceneHud_RefreshGameplayDisplay(s);
    s->previousCueGroup = 255;
    s->previousCueSelection = -1;
    one = 1;
    offset = 0xA5F;
    *(s8 *)((u8 *)s + offset) = -one;
    (*(struct SceneHudState **)&gFieldHud) = s;
    return 0;
}
