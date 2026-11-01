# Contact route recovery (080800A4)

Tags: #matching #scene-actors

SceneActorReaction_UpdateContactRouteRecovery matches all 1,008 bytes. Preparing
the actor clears the control flags and timers. Low health transfers control to
the defeat callback. A successful return route saves the entry callback with a
30-frame delay. Passing the control-flag field through a typed accessor lets
agbcc reuse its address across the drawable and route branches.
