# State actor eight-way burst

Tags: #matching #state-actor #particles

At frame three, the actor requests a four-particle group at each of four quarter-turn headings, then a two-particle group at each heading. Every request draws its own angle range from the shared random table. The two groups use different speeds, vertical ranges and lifetimes.

Keeping the outer variance in the caller and the inner group's fixed parameters at its call site reproduces all 964 bytes with stock agbcc. The sound-choice draw on state entry reads the low byte of a sample, whereas the eight spread draws read halfwords.
