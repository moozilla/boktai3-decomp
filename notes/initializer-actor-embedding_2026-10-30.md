# Initializer actor containment

Tags: #matching #monster #structs

Tentacle, Mummy and Axe now embed `SceneActorRegistryResetView` as `base`.
Their initializers derive the canonical pointer from that member; species fields
use the containing actor type. All three complete units remain byte-identical:

| Initializer | Bytes | Species fields |
|---|---:|---|
| TentacleActor_Init (080ABDCC) | 3,548 | first field at 0x8D0 |
| Mummy_Init (080DCC98) | 4,188 | existing base.field6E8; private field at 0x6F2 |
| Axe_Init (080FE68C) | 4,296 | first field at 0xE48 |

The canonical reset view is 0x6EC bytes under the target compiler. It cannot be
embedded before private fields beginning at 0x3D4 (Bat, Slime, Bee, Lun and Crow),
0x558 (Ghost), or 0x624 (the pursuit families). Those layouts remain narrow
views. Moving their state into offset subobject pointers changes allocation and
PRE; those nonmatching experiments remain outside the source tree. Extracting a
shorter common actor prefix would require migrating the existing shared accessors.

The named Bat palette and route helpers were already file-local in the supplied
baseline. Shared initializer headers contain no species-specific helper names.
