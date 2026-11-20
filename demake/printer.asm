; Game Boy Printer link receiver. Wire bytes use the external serial clock
; (SC=$80). DATA is reduced into tile count and darkness without a framebuffer.
; A packet contributes only after its checksum and RLE run both validate.
printer_init:
    ld hl,pr_state
    ld bc,27
    call clear
    xor a
    ldh [1],a
    ld a,128
    ldh [2],a
    ret

printer_byte:
    ld [pr_byte],a
    ld b,a
    ld a,[pr_state]
    and a
    jr nz,.magic2
    ld a,b
    cp 0x88
    jp nz,.zero
    jp .advance
.magic2:
    cp 1
    jr nz,.command
    ld a,b
    cp 0x33
    jp z,.advance
    cp 0x88
    jr z,.zero
    xor a
    ld [pr_state],a
    ret
.command:
    cp 2
    jr nz,.compression
    ld a,b
    ld [pr_command],a
    ld [pr_checksum_lo],a
    xor a
    ld [pr_checksum_hi],a
    ld [pr_raw_lo],a
    ld [pr_raw_hi],a
    ld [pr_ink_lo],a
    ld [pr_ink_hi],a
    ld [pr_run],a
    ld [pr_bad],a
    jp .advance
.compression:
    cp 3
    jr nz,.length_lo
    call pr_sum
    ld a,b
    ld [pr_compress],a
    cp 2
    jr c,.advance
    ld [pr_bad],a
    jp .advance
.length_lo:
    cp 4
    jr nz,.length_hi
    call pr_sum
    ld a,b
    ld [pr_length_lo],a
    ld [pr_packet_lo],a
    jp .advance
.length_hi:
    cp 5
    jr nz,.payload
    call pr_sum
    ld a,b
    ld [pr_length_hi],a
    ld [pr_packet_hi],a
    ld hl,pr_length_lo
    or [hl]
    jr nz,.advance
    ld a,7
    ld [pr_state],a
.zero:
    xor a
    ret
.advance:
    ld hl,pr_state
    inc [hl]
    xor a
    ret
.payload:
    cp 6
    jr nz,.check_lo
    call pr_sum
    ld a,[pr_command]
    cp 4
    call z,pr_data
    ld a,[pr_length_lo]
    sub 1
    ld [pr_length_lo],a
    ld b,a
    ld a,[pr_length_hi]
    sbc 0
    ld [pr_length_hi],a
    or b
    jp z,.advance
    xor a
    ret
.check_lo:
    cp 7
    jr nz,.check_hi
    ld a,b
    ld [pr_received_lo],a
    jp .advance
.check_hi:
    cp 8
    jr nz,.keepalive
    ld a,[pr_checksum_hi]
    cp b
    jr nz,.bad
    ld a,[pr_received_lo]
    ld hl,pr_checksum_lo
    cp [hl]
    jr nz,.bad
    ld a,[pr_run]
    ld hl,pr_bad
    or [hl]
    jr nz,.bad
    call pr_commit
    jr .ack
.bad:
    ld a,1
    ld [pr_status],a
.ack:
    ld a,9
    ld [pr_state],a
    ; SB for the NEXT transfer: identification during the keepalive byte.
    ld a,0x81
    ret
.keepalive:
    cp 9
    jr nz,.end
    ld a,10
    ld [pr_state],a
    ld a,[pr_status]
    ret
.end:
    xor a
    ld [pr_state],a
    ret

pr_sum:
    ld hl,pr_checksum_lo
    ld a,b
    add [hl]
    ld [hl+],a
    ret nc
    inc [hl]
    ret

pr_data:
    ld a,[pr_bad]
    and a
    ret nz
    ld a,[pr_compress]
    and a
    jr z,.one
    ld a,[pr_run]
    and a
    jr nz,.body
    ld a,b
    and 0x80
    ld [pr_repeat],a
    ld a,b
    and 127
    inc a
    bit 7,b
    jr z,.count
    inc a
.count:
    ld [pr_run],a
    ret
.body:
    ld a,[pr_repeat]
    and a
    jr z,.literal
    ld a,[pr_run]
    ld b,a
    xor a
    ld [pr_run],a
    jr pr_luminance
.literal:
    ld hl,pr_run
    dec [hl]
.one:
    ld b,1

; B is the number of copies of pr_byte. Count dark bits over the entire
; run in constant time: low planes have weight 1 and high planes weight 2.
pr_luminance:
    ld a,[pr_raw_lo]
    and 1
    add b
    srl a
    add b
    ld c,a
    ld hl,pr_raw_lo
    ld a,b
    add [hl]
    ld [hl+],a
    jr nc,.bounded
    inc [hl]
.bounded:
    ld a,[hl]
    cp 16
    jr c,.count
    ld [pr_bad],a
    ret
.count:
    ld a,[pr_byte]
    and 15
    ld e,a
    ld d,0
    ld hl,nibble_bits
    add hl,de
    ld b,[hl]
    ld a,[pr_byte]
    swap a
    and 15
    ld e,a
    ld hl,nibble_bits
    add hl,de
    ld a,[hl]
    add b
    ret z
    ld b,a
    ld a,[pr_ink_lo]
    ld l,a
    ld a,[pr_ink_hi]
    ld h,a
    ld e,c
.add:
    add hl,de
    dec b
    jr nz,.add
    ld a,l
    ld [pr_ink_lo],a
    ld a,h
    ld [pr_ink_hi],a
    ret
nibble_bits:
    db 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4

pr_commit:
    ld a,[pr_command]
    cp 1
    jr z,.init
    cp 4
    jr z,.data
    cp 2
    jp z,pr_print
    cp 15
    jr nz,.error
    ld hl,pr_busy
    ld a,[hl]
    and a
    ret z
    dec [hl]
    ld a,6
    jr nz,.status
    xor a
.status:
    ld [pr_status],a
    ret
.init:
    ld hl,pr_size_lo
    ld bc,4
    call clear
    xor a
    ld [pr_busy],a
    ld [pr_status],a
    ret
.error:
    ld a,1
    ld [pr_status],a
    ret
.data:
    ld a,[pr_raw_lo]
    and 15
    jr nz,.error
    ld hl,pr_raw_lo
    call pr_div16
    ld a,[pr_size_lo]
    ld l,a
    ld a,[pr_size_hi]
    ld h,a
    add hl,de
    ld a,h
    cp 8
    jr nc,.error
    ld [pr_size_hi],a
    ld a,l
    ld [pr_size_lo],a
    ld hl,pr_ink_lo
    call pr_div16
    ld a,[pr_total_lo]
    ld l,a
    ld a,[pr_total_hi]
    ld h,a
    add hl,de
    ld a,l
    ld [pr_total_lo],a
    ld a,h
    ld [pr_total_hi],a
    ld a,[pr_size_hi]
    and a
    jr nz,.ready
    ld a,[pr_size_lo]
    cp 40
    ret c
.ready:
    ld a,8
    ld [pr_status],a
    ret
pr_div16:
    ld a,[hl+]
    swap a
    and 15
    ld e,a
    ld a,[hl]
    swap a
    ld d,a
    and 0xf0
    or e
    ld e,a
    ld a,d
    and 15
    ld d,a
    ret

pr_print:
    ld a,[pr_packet_hi]
    and a
    ret nz
    ld a,[pr_packet_lo]
    cp 4
    ret nz
    ld a,[pr_size_lo]
    ld l,a
    ld a,[pr_size_hi]
    ld h,a
    or l
    ret z
    ld d,h
    ld e,l
    add hl,de
    add hl,de
    ld d,h
    ld e,l
    ld a,[pr_total_lo]
    ld l,a
    ld a,[pr_total_hi]
    ld h,a
    ld b,3
.level:
    call subtract_de
    jr c,.set
    dec b
    jr nz,.level
.set:
    ld a,b
    ld [photo_level],a
    ld a,1
    ld [photo_ready],a
    ld a,2
    ld [pr_busy],a
    ld a,6
    ld [pr_status],a
    ret
