#include "global.h"
#include "scene_actor_registry.h"

struct Node { struct Node *next; struct SceneActorRegistryActor *actor; };
struct Pt { s32 x:16; s32 y:16; s32 z:16; s32 w:16; };
struct Src { u16 a, b, c; };
struct Glob { u8 pad[0x5A0]; u32 regionX; u8 pad1[0x2C4]; u32 flags868; u8 pad2[0xA]; s16 f876; };

#define gNodes (*(struct Node **)0x020004AC)
#define gGlob (*(struct Glob **)0x02000710)
#define gCounter (*(u16 *)0x02000528)
#define gInput (*(u32 *)0x030053F4)
#define gIdx (*(u32 *)0x03005308)
#define gTable ((u16 *)0x0203B400)
#define gBase (*(u32 *)0x030042E4)
void sub_08219D38(void *);
void sub_08231500(void *, s32, void *, void *, void *, s32, s32);
u8 sub_08071F50(void *, void *);
void sub_08071E00(u32, u32, u32, u32, u32);
void sub_08215284(void *, u32);

struct Sub { u32 pad0, pad1, flags; };
static inline struct Sub *subOf(void *d) { return (struct Sub *)((u8 *)d + 0x20); }
static inline u8 bad(struct SceneActorRegistryActor *a, vu16 *f2D4, u32 *f2D0)
{
    if ((*f2D4 & 0x40) || (*f2D0 & 0x20000) || a->health <= 0) return TRUE;
    return FALSE;
}
static inline u8 isNull(void *p) { if (p == 0) return TRUE; return FALSE; }
static inline u8 isSet(u32 v, u32 m) { if (v & m) return TRUE; return FALSE; }
static inline u8 isSet16(u16 v, u16 m) { if (v & m) return TRUE; return FALSE; }
static inline u8 isOne(u16 v) { if (v == 1) return TRUE; return FALSE; }
static inline u8 isEq(u32 a, u16 b) { if (a == b) return TRUE; return FALSE; }

void sub_08070FA8(struct SceneActorRegistryManager *mgr)
{
    struct Node *n = gNodes;
    struct SceneActorRegistryActor *a;
    vu16 *f2D4;
    u8 z;
    u32 *f2D0;
    u32 *f2CC;
    u16 *p2B8;
    if (!n) return;
    a = n->actor;
    while (a) {
        if (isSet16(a->flags2D6, 0x1000)) {
            void *c = a->context;
            n = n->next;
            sub_0824923C(c, a->destroy);
            sub_08219D38(c);
            a = n->actor;
            continue;
        }
        sub_0824923C(a->context, a->update);
        z = isNull(a->object);
        f2D4 = &a->flags2D4;
        if (z) goto done;
        z = isSet(gInput, 0x4800);
        f2D4 = &a->flags2D4;
        if (z) goto done;
        if (*f2D4 & 0x4800) goto done;
        f2D0 = &a->flags2D0;
        if (*f2D0 & 2) goto done;
        if (a->enabled == 0) goto done;
        f2CC = &a->flags2CC;
        if (*f2CC & 2) goto done;
        if (!(gGlob->flags868 & 2)) goto done;
        if (!isEq(gGlob->regionX, a->regionId)) goto done;
        z = bad(a, f2D4, f2D0);
        if (z) goto done;
        if (gGlob->f876 <= 0) goto done;
        z = 0;
        if (gCounter == 1) z = 1;
        if (z) goto done;
        {
            struct Pt A, B, C;
            s32 i;
            a->field178 = 0;
            p2B8 = &a->field2B8;
            *p2B8 = 0;
            if (a->drawableKind == 0)
                *(u32 *)a->drawable &= ~1;
            else
                subOf(a->drawable)->flags &= ~1;
            a->health = 0;
            if (*f2D4 & 0x2000) {
                for (i = 2; i >= 0; i--) {
                    u32 idx = gIdx;
                    idx = (idx + 1) & 0x3FF;
                    A.x = (gTable[idx] & 0xF) - 7;
                    idx = (idx + 1) & 0x3FF;
                    A.y = (gTable[idx] & 0x1F) - 0x10;
                    idx = (idx + 1) & 0x3FF;
                    gIdx = idx;
                    A.z = (gTable[idx] & 0xF) - 7;
                    B.x = 5;
                    B.y = 10;
                    B.z = 5;
                    C.x = ((u16 *)a->effectOrigin)[0];
                    C.y = ((u16 *)a->effectOrigin)[1] + 0x80;
                    C.z = ((u16 *)a->effectOrigin)[2];
                    sub_08231500(a->effectConfig, 3, &C, &A, &B, 0x28, 0x8);
                }
                a->fieldAA = 0;
                *p2B8 = 0;
            }
            *f2D4 &= ~0x2000;
            *f2CC |= 0x1000;
        }
    done:
        if (!(*f2D4 & 0x4000) && isSet(mgr->flags, 0x2000)
            && (a->category == 10 || a->category == 23)) {
            if (!(*f2CC & 0x2008000)) {
                if (a->drawableKind == 0)
                    ((u16 *)a->drawable)[0x32 / 2] = a->paletteBank;
                else
                    *(u16 *)((u8 *)a->drawable + 0x20 + 0x3a) = a->paletteOffset;
            }
            if (sub_08071F50(mgr, a)) {
                if (isSet(mgr->flags, 0x20000)) {
                    u32 s;
                    mgr->flags &= 0xFFFDDFFF;
                    *f2D0 &= 0xFFDFFFFF;
                    s = a->basePaletteBank + a->paletteOffset;
                    if (a->drawableKind == 0)
                        sub_08215284((u8 *)a->drawable + 0x2c, s);
                    else
                        *(u32 *)((u8 *)a->drawable + 0x68) = gBase + (s << 5);
                } else if (*f2D0 & 0x200000) {
                    sub_08071E00(a->basePaletteBank + a->paletteOffset, a->paletteBank, 0, a->category, 0);
                    *f2D0 &= 0xFFDFFFFF;
                } else {
                    sub_08071E00(a->paletteBank, a->basePaletteBank + a->paletteOffset, 0, a->category, 0);
                    *f2D0 |= 0x200000;
                }
            }
        }
        n = n->next;
        a = n->actor;
    }
}
