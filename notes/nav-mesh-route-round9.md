# NavMesh_FindRoute

The guarded route finder reaches 89.9% fuzzy from 27.9%. It chooses the first
region containing the destination, gathers up to six regions at each endpoint,
and breaks equal graph distances by the squared distance from the origin to the
candidate waypoint. Each full-vector copy preserves the fourth halfword. The
waypoint-only update writes three components and leaves that halfword alone.

The original 500-case comparison covered cached, failed, direct and waypoint
routes. The reconstructed candidate still has 62 realigned differences, three
when registers are ignored. It is four bytes longer than the 1,212-byte ROM body.
