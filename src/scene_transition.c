#include "scene_transition.h"
#include "tether_presentation.h"
#include "background_controller.h"

struct SpriteEffectController;

extern const s16 gUnk_086149C4[];

struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *,
                                                             const struct ContactPosition *, s32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             u32, s32, s32, s32, s32, s32, s32);

static inline s32 SpriteEffect_ScaleFixed(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}

static inline void SpriteEffect_SetPolarVelocity(struct ContactPosition *vel, s32 angle, s32 speed)
{
    vel->x = SpriteEffect_ScaleFixed(gUnk_086149C4[angle + 0x40] * speed);
    vel->y = SpriteEffect_ScaleFixed(gUnk_086149C4[angle] * speed);
    vel->z = 0;
}

// Tentative: emits the transition's sixteen-particle burst.

void SceneTransition_SpawnParticleBurst(s32 x, s32 y, s32 flags)
{
    struct ContactPosition position, velocity;
    s32 i;
    for (i = 0; i < 16; i++) {
        const u16 *table;
        u32 *seed;
        u16 index;
        u32 mask;
        s32 angle, random;
        struct ContactPosition *pos;
        table = gRandomTable;
        seed = &gRandomIndex;
        index = *seed + 1;
        mask = 0x3FF;
        index &= mask;
        angle = table[index];
        angle >>= 4;
        angle &= 0xF;
        index = (index + 1) & mask;
        *seed = index;
        random = table[index];
        random >>= 4;
        random &= 7;
        SpriteEffect_SetPolarVelocity(&velocity, angle + 0x18, random + (1 << (i >> 2)) + 1);
        pos = &position;
        pos->x = x;
        pos->y = y;
        pos->z = 0;
        SpriteEffectController_Create(&position, &velocity, 0x1C1E, 0xD43F, 0, 0x10, 0x11, 1, 2,
                                      flags, 0, 90, 0, 0, 0, 0, -4, -4);
        table = gRandomTable;
    }
}

// confidence=low: spawns a sprite effect at (x, y) flying off at a random angle and speed;

// bit 0 of flags picks the 0x200 attribute instead of 0x400.

void SceneTransition_SpawnFlyingParticle(s32 x, s32 y, u32 flags)
{
    struct ContactPosition position, velocity;
    struct ContactPosition *pos = &position;
    const u16 *table;
    u32 *seed;
    u16 index;
    u32 mask;
    s32 angle, random;
    u32 attribute;
    pos->x = x;
    pos->y = y;
    pos->z = 0;
    table = gRandomTable;
    seed = &gRandomIndex;
    index = *seed + 1;
    mask = 0x3FF;
    index &= mask;
    angle = table[index];
    angle >>= 4;
    angle = angle & 0x1F;
    index = (index + 1) & mask;
    *seed = index;
    random = table[index];
    random >>= 4;
    random &= 3;
    SpriteEffect_SetPolarVelocity(&velocity, angle + 0x10, random + 3);
    flags &= 1;
    attribute = 0x400;
    if (flags)
        attribute = 0x200;
    attribute |= 0x10;
    SpriteEffectController_Create(&position, &velocity, 0x8639, 0x75F6, 0, attribute, 9, 0x153, 2,
                                  0, 0, 30, 0, 0, 0, 0, 0, 0);
}

void BgGfx_LoadTilemapAtPosition(u32, s32, s32);

void Registry6A37_UpdateBackgroundScroll(struct SceneTransition *transition)
{
    BgGfx_LoadTilemapAtPosition(3, transition->scroll.x, transition->scroll.y);
}

// Provisional object-family interpretation.

void ContactImpactEffect_BeginInterpolatedPosition(void *, s16 *, s16 *, s32, s32, s32);

void SceneTransition_BeginOffsetMove(struct SceneTransition *transition)
{
    s16 a[3], b[3];
    s16 *q = a;
    s32 x = 0x58, y = -0x10;
    s32 zero = 0;
    a[0] = x;
    q[1] = y;
    q[2] = zero;
    {
        s16 *r = b;
        s32 x = 0x38, y = 0x30;
        r[0] = x;
        r[1] = y;
        r[2] = zero;
    }
    ContactImpactEffect_BeginInterpolatedPosition(transition->target, (s16 *)&a, (s16 *)&b, 0x18, 1,
                                                  0x78);
    transition->update = Registry6A37_UpdateDisplayHold;
    transition->timer = zero;
}

// Provisional object-family interpretation.

void PositionMotion_InitTimedInterpolation(void *, void *, s16 *, s16 *, u16);

void SceneTransition_BeginVectorTween(struct SceneTransition *transition)
{
    s16 a[3], b[3];
    s16 *q = a;
    s32 x = 0xc0, y = 0x60;
    s32 zero = 0;
    a[0] = x;
    q[1] = y;
    q[2] = zero;
    {
        s16 *r = b;
        s32 x = 0xa7, y = 0x4b;
        r[0] = x;
        r[1] = y;
        r[2] = zero;
    }
    PositionMotion_InitTimedInterpolation((u8 *)&transition->motion, &transition->scroll, a, b,
                                          0x5a);
    transition->update = Registry6A37_UpdateSecondScrollTransition;
    transition->timer = zero;
}

// Provisional object-family interpretation.

void SceneTransition_BeginFirstScroll(struct SceneTransition *transition)
{
    s16 a[3], b[3];
    s16 *q = a;
    s32 x = 0x15e, y = 0xc4;
    s32 zero = 0;
    a[0] = x;
    q[1] = y;
    q[2] = zero;
    {
        s16 *r = b;
        s32 x = 0xc0, y = 0x60;
        r[0] = x;
        r[1] = y;
        r[2] = zero;
    }
    PositionMotion_InitTimedInterpolation((u8 *)&transition->motion, &transition->scroll, a, b,
                                          0xf);
    for (zero = 0; zero <= 7; zero++) {
        SceneTransition_SpawnFlyingParticle(0x50, 0x30, zero);
    }
    SceneTransition_SpawnParticleBurst(0, 0, transition->particleFlags);
    {
        s32 zero;
        ScreenTransition_SendCommand(0, 4, 0x1f, 0x1f, 0x1f, 0xFFFF, zero = 0);
        transition->update = Registry6A37_UpdateFirstScrollTransition;
        transition->timer = zero;
    }
}

void Sound_PlaySong(u32);

void Registry6A37_StartSoundPhase(struct SceneTransition *transition)
{
    Sound_PlaySong(0x50F);
    transition->update = SceneTransition_UpdateShakingBackground;
    transition->timer = 0;
}

void Registry6A37_StartFadeIn(struct SceneTransition *transition)
{
    s16 *v = (&transition->scroll.x);
    s16 a = 0x15e, b = 0xc4;
    s32 z = 0;
    (&transition->scroll.x)[0] = a;
    v[1] = b;
    v[2] = z;
    ScreenTransition_SendCommand(0, 6, 4, 4, 4, 0xFFFF, z);
    transition->update = Registry6A37_UpdateFadeIn;
    transition->timer = z;
}

void Registry6A37_StartFadeOut(struct SceneTransition *transition)
{
    ScreenTransition_SendCommand(1, 6, 4, 4, 4, 0xFFFF, 0);
    transition->update = Registry6A37_UpdateFadeOut;
    transition->timer = 0;
}

void Registry6A37_UpdateDisplayHold(struct SceneTransition *transition)
{
    Registry6A37_UpdateBackgroundScroll(transition);
    if (transition->timer > 0xb3) {
        Registry6A37_StartFadeOut(transition);
    }
    transition->timer++;
}

void MotionTrack_Update(u8 *);

void Registry6A37_UpdateSecondScrollTransition(struct SceneTransition *transition)
{
    s32 r;
    Registry6A37_UpdateBackgroundScroll(transition);
    MotionTrack_Update((u8 *)&transition->motion);
    r = 0;
    if (transition->motion.state == 2)
        r = 1;
    if (r)
        SceneTransition_BeginOffsetMove(transition);
    transition->timer++;
}

void Registry6A37_UpdateFirstScrollTransition(struct SceneTransition *transition)
{
    s32 r;
    Registry6A37_UpdateBackgroundScroll(transition);
    MotionTrack_Update((u8 *)&transition->motion);
    r = 0;
    if (transition->motion.state == 2)
        r = 1;
    if (r)
        SceneTransition_BeginVectorTween(transition);
    transition->timer++;
}

s32 sub_08249350(s32, s32);

// confidence=low: scrolls the background and shakes it by a growing random amount; after 120

// frames starts the first scroll transition.

void SceneTransition_UpdateShakingBackground(struct SceneTransition *transition)
{
    u16 *table;
    u32 *seed;
    u16 index;
    u32 mask;
    s32 timer, range, random;
    Registry6A37_UpdateBackgroundScroll(transition);
    table = gRandomTable;
    seed = &gRandomIndex;
    index = *seed + 1;
    mask = 0x3FF;
    index &= mask;
    *seed = index;
    random = table[index];
    random >>= 4;
    timer = transition->timer;
    range = (timer >> 4) + 3;
    transition->scroll.x = sub_08249350(random, range) + 0x15E;
    index = (index + 1) & mask;
    *seed = index;
    random = table[index];
    random >>= 4;
    transition->scroll.y = sub_08249350(random, range) + 0xC4;
    if (timer > 0x77)
        SceneTransition_BeginFirstScroll(transition);
    transition->timer++;
}

void Registry6A37_UpdateFadeOut(struct SceneTransition *transition)
{
    Registry6A37_UpdateBackgroundScroll(transition);
    transition->timer++;
    if (transition->timer > 0x3f) {
        if (transition->exitScript) {
            Script_Call(transition->exitScript, 0);
            Task_Kill(transition->target);
            Task_Kill(transition);
        } else {
            Registry6A37_StartFadeIn(transition);
        }
    }
}

void Registry6A37_UpdateFadeIn(struct SceneTransition *transition)
{
    Registry6A37_UpdateBackgroundScroll(transition);
    transition->timer++;
    if (transition->timer > 0x3f) {
        Registry6A37_StartSoundPhase(transition);
    }
}

void Registry6A37_Update(struct SceneTransition *transition)
{
    if (transition->update != 0)
        Callback_InvokeActor(transition, transition->update);
}

void Registry6A37_OnDestroy(void *transition)
{
}

// Provisional object-family interpretation.

s32 SunClock_GetPhase(void);

void *ScriptProc_Start(u16, u16);

void *SceneTransition_StartSunScript(void)
{
    s32 n = SunClock_GetPhase();
    u16 key;
    switch (n) {
        case 1:
        case 2:
            key = 0x2877;
            break;
        case 3:
        case 4:
            key = 0xe411;
            break;
        default:
            key = 0x225e;
            break;
    }
    return ScriptProc_Start(0x92b3, key);
}

void *SceneTransition_StartSunScript(void);

void SceneTransition_LoadBackground(struct SceneTransition *screen)
{
    void *resource;
    s32 layer;
    u16 tile;
    u16 firstTile;
    struct ScreenPaletteResource *palette;
    {
        u16 *control = &gDispCntShadow;
        u32 mask = 0xFFFFF0FF;
        u16 flags = *control;
        mask &= flags;
        {
            u16 enabled = 0x800;
            *control = mask | enabled;
        }
    }
    resource = sub_08216E54(0xB65E);
    screen->background = resource;
    layer = 0;
    Bg_SetupLayers(2, 0, resource, 0, 0, 1, &layer);
    Bg_SetPriority(3, 3);
    resource = ScriptProc_Start(0xCEEF, 0x8E32);
    {
        struct BackgroundResource *tiles = resource;
        tile = 0xD759;
        firstTile = tiles->size;
    }
    sub_08215D40(2, resource, 1, &tile, firstTile);
    palette = SceneTransition_StartSunScript();
    CpuSet(palette->colors, gBgPaletteBuffer, 0x04000080);
}

void *RegistryF3E4_CreateHidden(s32);

// confidence=low: sets up the layers, reads keyword 'e' as the script, starts the fade-in and

// creates a hidden panel. Declared s32 with no return statement, as in the ROM.

s32 SceneTransition_Initialize(struct SceneTransition *transition, void *scriptArguments)
{
    s32 value;
    SceneTransition_LoadBackground(transition);
    transition->scroll.x = 0x15E;
    transition->scroll.y = 0xC4;
    if ((value = Script_SeekToKeyword(0x65)))
        transition->exitScript = Script_GetValue();
    else
        transition->exitScript = value;
    Registry6A37_StartFadeIn(transition);
    {
        u32 zero = 0;
        transition->motion.position = zero;
        transition->motion.state = zero;
        transition->motion.mode = zero;
        transition->motion.timer = zero;
    }
    transition->target = RegistryF3E4_CreateHidden(2);
    transition->particleFlags = 1;
}

void *Task_Create(u32, u32);

void *Registry6A37_Create(void *a)
{
    void *transition = Task_Create(0xb, 0x70);
    if (transition != 0) {
        Task_SetCallbacks(transition, Registry6A37_Update, Registry6A37_OnDestroy);
        if (SceneTransition_Initialize(transition, a) < 0) {
            Task_Kill(transition);
            return 0;
        }
    }
    return transition;
}
