# Spider contact windup

Tags: 080A28A8 #spider #depin

`Spider_UpdateContactWindup` matches all 400 bytes without pins or barriers.
The existing state and control helpers replace the handwritten byte stores.
A halfword flag view preserves the address, operand order and width of both
mask updates in the transition to pursuit.
