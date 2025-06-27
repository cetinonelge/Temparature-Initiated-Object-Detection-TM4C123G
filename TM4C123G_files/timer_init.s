    AREA Timer5Code, CODE, READONLY
    EXPORT Timer5_Init
    EXPORT Timer5_DelayUs
    EXPORT Timer5_DelayMs
    EXTERN isDeepSleepPressed   ; Declare external variable from main.c

; ==================== Timer5 Initialization ====================
Timer5_Init
    LDR     R0, =0x400FE604      ; SYSCTL_RCGCTIMER_R
    LDR     R1, [R0]
    ORR     R1, R1, #0x20        ; Enable Timer5 clock
    STR     R1, [R0]

WaitReady
    LDR     R0, =0x400FEA04      ; SYSCTL_PRTIMER_R
    LDR     R1, [R0]
    TST     R1, #0x20            ; Check if Timer5 is ready
    BEQ     WaitReady            ; Wait until ready

    LDR     R0, =0x4003500C      ; TIMER5_CTL_R
    MOV     R1, #0
    STR     R1, [R0]             ; Disable Timer5 during configuration

    LDR     R0, =0x40035000      ; TIMER5_CFG_R
    STR     R1, [R0]             ; Configure as 32-bit timer

    LDR     R0, =0x40035004      ; TIMER5_TAMR_R
    MOV     R1, #0x02            ; Periodic mode
    STR     R1, [R0]

    LDR     R0, =0x40035028      ; TIMER5_TAILR_R
    LDR     R1, =0xFFFFFFFF
    STR     R1, [R0]             ; Set max load value

    LDR     R0, =0x4003500C      ; TIMER5_CTL_R
    MOV     R1, #1
    STR     R1, [R0]
    BX      LR

; ==================== Timer5 Delay in Microseconds ====================
Timer5_DelayUs
    PUSH    {R4, R5, R6, LR}     ; Save registers

    LDR     R1, =0x40035028      ; TIMER5_TAILR_R
    LDR     R2, =0x40035024      ; TIMER5_ICR_R
    LDR     R3, =0x4003500C      ; TIMER5_CTL_R
    LDR     R4, =0x4003501C      ; TIMER5_RIS_R
    LDR     R6, =isDeepSleepPressed  ; Load address of isDeepSleepPressed

    MOV     R5, #16              ; Load 16 into R5
    MUL     R0, R0, R5           ; Convert microseconds to ticks
    SUB     R0, R0, #1
    STR     R0, [R1]             ; Load value into TIMER5_TAILR_R

    MOV     R0, #1
    STR     R0, [R2]             ; Clear timeout flag

    LDR     R0, [R3]
    ORR     R0, R0, #1
    STR     R0, [R3]             ; Start Timer5

WaitForTimeout
    LDR     R0, [R4]             ; Read TIMER5_RIS_R
    TST     R0, #1
    BNE     ClearTimeoutFlag     ; If timeout flag is set, clear it

    ; Check if isDeepSleepPressed is set
    LDRB    R0, [R6]             ; Load the value of isDeepSleepPressed
    CMP     R0, #1               ; Compare with 1
    BEQ     ExitDelay            ; Exit if isDeepSleepPressed is high

    B       WaitForTimeout       ; Otherwise, continue waiting

ClearTimeoutFlag
    MOV     R0, #1
    STR     R0, [R2]             ; Clear timeout flag

ExitDelay
    POP     {R4, R5, R6, PC}     ; Restore registers and return

; ==================== Timer5 Delay in Milliseconds ====================
Timer5_DelayMs
    PUSH    {R4, R5, LR}         ; Save registers

    MOV     R5, #1000            ; Load 1000 into R5
    MUL     R0, R0, R5           ; Convert milliseconds to microseconds
    BL      Timer5_DelayUs       ; Call microsecond delay
    POP     {R4, R5, PC}         ; Restore and return

    END
