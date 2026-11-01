# NativeEffect_CreateFromProfile

The profile-driven creator matches all 808 bytes. The capacity check packs
the frame and group into a 16-bit ID. After choosing a free slot, the creator
reuses the incoming group variable for the new ID. That makes agbcc preserve
the unmasked input separately until the final calculation.

Type 2 selects the collision callback at a threshold of 60. Both branches
assign the same damage local through the same helper. This lets agbcc remove
the repeated test and double the damage in the collider argument register.
