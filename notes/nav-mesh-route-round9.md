# NavMesh_FindRoute

The route finder matches all 1,212 bytes. It gathers up to six regions at
each endpoint, then chooses a pair of nodes by graph distance. Equal graph
distances are resolved by squared distance to the origin. A direct route
copies the complete position, including the fourth halfword. A graph
waypoint writes only X, Y and Z.

The destination node slot advances after the inner membership scan. Giving
this cursor its own update keeps agbcc from moving the increment before
the scan and spilling its result. The index declaration also controls the
order of the two slots used later during the node-pair search.

Tags: #navigation #matching #round9b
