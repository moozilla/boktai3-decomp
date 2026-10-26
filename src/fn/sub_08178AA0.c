#include "global.h"
#include "scene_hud_display.h"

#define gSceneHud            gFieldHud
#define gSceneHudOwnerSource gUnk_020004B4
#define gPartyActors         gUnk_02000580
struct SceneHudOwnerSource {
    u8 unknown0[0x18];
    s16 index;
};
extern struct SceneHudOwnerSource *gSceneHudOwnerSource;
extern struct Player *gPartyActors[];
extern struct SceneHudState *gSceneHud;

void Hud_LoadLayoutResources(struct SceneHudState *);
void SceneHud_InitGameplayResources(struct SceneHudState *);
void SceneHud_RefreshGameplayDisplay(struct SceneHudState *);
static inline s32 OwnerIndex(void)
{
    if (!gSceneHudOwnerSource)
        return -1;
    return gSceneHudOwnerSource->index;
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
    gSceneHud = s;
    return 0;
}
