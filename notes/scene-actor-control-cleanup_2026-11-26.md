# Scene actor control cleanup

Tags: #matching #types #phase3 #scene

Control diagnostics fell from 91 lines to 84 (7.7%) and charged constant
uses from 91 to 83 (8.8%), with zero errors. The changed translation units
match under the stock compiler. The full stored-motion unit matches all
3268 bytes. Exact code coverage remains 100%, a change of zero percentage
points.

- The control slot and stored-motion unit use declared player and game state
  fields, actor callbacks, tracking and resource ownership. The visibility
  tick shares one operation for the two drawable/shadow alternatives.
- Recovery contact (08082648) uses the native HUD fields, typed tracking
  query and existing state transitions. The contact power floor (080829C0)
  uses ContactCollider and its response parameter.
- Lift, ground bob and collision response (080837D4, 080838AC, 08084180,
  08084210) use actor state and tracking fields, with the existing
  availability and drawable operations where applicable.
- Position-curve startup (08083E98) captures the typed vector address
  before assigning CpuSet's fill value. The ordinary typed expression in
  the call would build the address one instruction later.

The direct context read and split contact union at 08082648 alter allocation.
The stored control point at 08084284 still uses its narrow input view.
The typed position pointer changes the address form in its callers. Folding
the ground bob's single-pass body into a direct store changes the local
registers while preserving instruction shape. The two visibility request
branches still differ from a shared transition because their load and branch
ordering changes in the shared form.
