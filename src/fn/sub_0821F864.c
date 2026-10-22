#include "global.h"

struct Pos {
    u16 x, pad2, z;
};
struct NavRegion {
    u8 pad0[4];
    u8 links[12];
};
struct NavLink {
    u8 pad0[6];
    u8 regions[6];
};
struct NavMesh {
    u16 count, unknown2;
    u32 unknown4, regionOffset, linkOffset;
};
struct NavResource {
    u16 count, unknown2;
    u32 offsets[1];
};
struct NavQuery {
    u8 pad[4];
    u16 mesh;
    u16 region;
};
u32 NavMesh_IsPointInsideRegion(struct NavRegion *, struct Pos *, u32);

static inline struct NavMesh *GetMesh(struct NavResource *res, u32 idx)
{
    u32 sh = idx << 2;
    u32 *offsets;
    return (void *)((u8 *)res + *(u32 *)((u32)(offsets = res->offsets) + sh));
}

void sub_0821F864(struct NavResource *res, struct NavQuery *q, struct Pos *pos)
{
    struct NavMesh *mesh = GetMesh(res, q->mesh);
    struct NavRegion *regions = (void *)((u8 *)mesh + mesh->regionOffset);
    struct NavLink *links;
    struct NavRegion *region;
    s32 i, j;

    if (NavMesh_IsPointInsideRegion(regions, pos, q->region))
        return;
    links = (void *)((u8 *)mesh + mesh->linkOffset);
    region = &regions[q->region];
    for (i = 0; i < 12 && region->links[i] != 0xff; i++) {
        struct NavLink *link;
        link = &links[region->links[i]];
        for (j = 0; j < 6; j++) {
            u8 *base = (u8 *)link + 6;
            u8 *k = base + j;
            if (*k == 0xff)
                j = 6;
            else if (*k != q->region && NavMesh_IsPointInsideRegion(regions, pos, *k)) {
                q->region = *k;
                return;
            }
        }
    }
    for (i = 0; i < mesh->count; i++) {
        if (i != q->region && NavMesh_IsPointInsideRegion(regions, pos, i)) {
            q->region = i;
            return;
        }
    }
    for (i = 0; i < res->count; i++) {
        if (i == q->mesh)
            continue;
        mesh = GetMesh(res, i);
        regions = (void *)((u8 *)mesh + mesh->regionOffset);
        for (j = 0; j < mesh->count; j++) {
            if (NavMesh_IsPointInsideRegion(regions, pos, j)) {
                q->mesh = i;
                q->region = j;
                return;
            }
        }
    }
}
