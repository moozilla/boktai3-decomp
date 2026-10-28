#ifndef GUARD_OBJ_TILE_UPLOAD_H
#define GUARD_OBJ_TILE_UPLOAD_H

#include "global.h"

/* RAM objects; each row of symbols/ram.csv with header include/obj_tile_upload.h. */

struct ObjTileUploadRequest {
    u32 src;
    union {
        u32 desc;
        struct {
            u16 size, count;
        } rows;
    } transfer;
};
extern struct ObjTileUploadRequest gObjTileUploadRequests[];

#endif
