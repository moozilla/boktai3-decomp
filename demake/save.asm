; Two battery SRAM records at $a000 and $a100. The last written byte is the
; commit marker. Generation and Fletcher sums protect the previous checkpoint
; across interrupted writes. The tag identifies the ordered scene set and ABI.
save_enable:
    ld a,10
    ld [0],a
    xor a
    ld [0x4000],a
    ret
save_disable:
    xor a
    ld [0],a
    ret

; Add each payload byte into D, then D into E (both modulo 256).
; HL advances to the stored sums.
save_sum:
    ld b,save_body_bytes
    ld de,0
.loop:
    ld a,[hl+]
    add d
    ld d,a
    add e
    ld e,a
    dec b
    jr nz,.loop
    ret

save_valid:
    ld a,[hl+]
    cp 0xa5
    ret nz
    push hl
    ld de,save_tag
    ld b,4
.tag:
    ld a,[de]
    cp [hl]
    jr nz,.wrong
    inc de
    inc hl
    dec b
    jr nz,.tag
    pop hl
    call save_sum
    ld a,[hl+]
    cp d
    ret nz
    ld a,[hl]
    cp e
    ret
.wrong:
    pop hl
    ret

save_find:
    xor a
    ld [save_slot],a
    ld [save_generation],a
    call save_enable
    ld hl,0xa000
    call save_valid
    jr nz,.second
    ld a,0xa0
    ld [save_slot],a
    ld a,[0xa005]
    ld [save_generation],a
.second:
    ld hl,0xa100
    call save_valid
    jr nz,save_disable
    ld a,[save_slot]
    and a
    jr z,.use
    ld a,[0xa105]
    ld hl,save_generation
    sub [hl]
    jr z,save_disable
    bit 7,a
    jr nz,save_disable
.use:
    ld a,0xa1
    ld [save_slot],a
    ld a,[0xa105]
    ld [save_generation],a
    jr save_disable

save_store:
    ld hl,save_tag
    ld de,0xc801
    ld bc,4
    call copy
    ld hl,save_generation
    inc [hl]
    ld a,[hl]
    ld [de],a
    inc de
    ld hl,save_fields
    ld b,save_field_count
.gather:
    ld a,[hl+]
    push hl
    ld h,0xc1
    ld l,a
    ld a,[hl]
    ld [de],a
    inc de
    pop hl
    dec b
    jr nz,.gather
    ld hl,pickups
    ld bc,room_count
    call copy
    ld hl,0xc801
    call save_sum
    ld a,d
    ld [hl+],a
    ld [hl],e
    call save_enable
    ld a,[save_slot]
    cp 0xa0
    ld a,0xa0
    jr nz,.slot
    inc a
.slot:
    ld [save_slot],a
    ld d,a
    ld e,0
    xor a
    ld [de],a
    inc de
    ld hl,0xc801
    ld b,save_body_bytes+2
save_copy_byte:
    ld a,[hl+]
    ld [de],a
    inc de
    dec b
    jr nz,save_copy_byte
    ld e,0
    ld a,0xa5
    ld [de],a
    jp save_disable

save_restore:
    call save_enable
    ld a,[save_slot]
    ld d,a
    ld e,6
    ld hl,save_fields
    ld b,save_field_count
.restore:
    ld a,[hl+]
    push hl
    ld l,a
    ld h,0xc1
    ld a,[de]
    ld [hl],a
    inc de
    pop hl
    dec b
    jr nz,.restore
    ld h,d
    ld l,e
    ld de,pickups
    ld bc,room_count
    call copy
    jp save_disable
