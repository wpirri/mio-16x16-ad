; ============================================================================
; PROGRAM: MIO16x16/AD
; Dispositivo de interface I/O 16 Entradas, 16 Salidas TTL
; 4 Entradas analogicas
; Medicion de temperatura on board
; conexion teclado
; i2c / SPI
; Serial 1200,N,8,1 (configurable 1200 a 19200)
; Walter Gabriel Pirri
; ----------------------------------------------------------------------------
; PIC 16F877 /877A /887
;
; SELECCIONAR EL PROCESADOR EN: Configure -> Select Device...
;
;     PORT A: 0 - A/D Onboard
;             1 - A/D Ext. 1
;             2 - A/D Ext. 2
;             3 - A/D Ext. 3
;             4 - Dig Ext. 1
;             5 - I2C/SPI - SS
;
;     PORT B: 0 - INT
;             1 - Display EN
;             2 - Display RS
;             3 - Display RW
;             4 - Disable Entrada 1
;             5 - Enable salida 2
;             6 - Enable salida 1
;             7 - Disable Entrada 1
;
;     PORT C: 0 - PWM CK
;             1 - PWM2
;             2 - PWM1
;             3 - I2C/SPI - SCK/SCL
;             4 - I2C/SPI - SDI/SDA
;             5 - I2C/SPI - SDO
;             6 - Serial Tx
;             7 - Serial Rx
;
;     PORT D: BUS de datos
;
;     PORT E: 0 - Status led
;             1 - Buzzer
;             2 - Disable Keyboard
;
; ----------------------------------------------------------------------------
; Xtal 3,6864 MHz
; div interna /4 /256 (normal) = 278us
; ----------------------------------------------------------------------------
; prescaler asignado a WDT
; divisores timer(3,6864 Mhz) / 4 / 256 / 36 / 100
;                                          \_ 10ms
;                                                \_ 1 seg
; ciclo de instruccion  1,08us
; int timer             278us
; ============================================================================
; Include processor definitions
 ifdef __16F877
  include "p16f877.inc"
 endif

 ifdef __16F877A
  include "p16f877a.inc"
 endif

 ifdef __16F887
  include "p16f887.inc"
 endif

; ----------------------------------------------------------------------------
; Configuration bits
 ifdef __16F887
  __CONFIG _CONFIG1, _XT_OSC & _WDT_ON & _CP_OFF & _PWRTE_ON & _LVP_OFF
  __CONFIG _CONFIG2, _WRT_OFF
 else
  __CONFIG _XT_OSC & _WDT_ON & _CP_OFF & _PWRTE_ON & _LVP_OFF
 endif
; ============================================================================
  include "const.inc"
  include "vars.inc"
  include "page1.inc"
  include "page2.inc"
; ============================================================================
; Inicio de corrida
; ============================================================================
  org 0x00              ; reset vector
  goto    Init
; ============================================================================
; Tratamiento del vector de INTERRUPCION
; ============================================================================
  org 0x04              ; interrupt vector
  bcf       INTCON,GIE    ;inhabilito las interrupciones
; >>>> Save context >>>>
  movwf     w_saved       ;w --> w_saved register
  swapf     STATUS,w      ;Swap status to be saved into W
  clrf      STATUS        ;bank 0, regardless of current bank,Clears IRP,RP1,RP0
  movwf     status_saved  ;Save status to bank zero status_saved register
  movf      FSR,w         ;Guardo el registro de direccionamiento indirecto
  movwf     fsr_saved     ;Save FSR into fsr_saved
  movf      PCLATH,w      ;Only required if using pages 1, 2 and/or 3
  movwf     pclath_saved  ;Save PCLATH into pclath_saved
  clrf      PCLATH        ;Page zero, regardless of current page
; <<<< Save context <<<<

; >> interrupt code >>

 ERRORLEVEL -306
  ; << Interrupcion de timer0 en Pagina 1
  PAGESEL TimerInt
  btfsc   INTCON,T0IF    ;interrupcion por timer0
  call    TimerInt
  PAGESEL $
  ; << Interrupcion de timer2 en Pagina 1
  PAGESEL IRCodeInt
  btfsc   PIR1,TMR2IF    ;interrupcion por timer2
  call    IRCodeInt
  PAGESEL $
  ; << Interrupcion externa en Pagina 1
  PAGESEL ExternInt
  btfsc   INTCON,INTF   ;interrupcion externa RB0
  call    ExternInt
  PAGESEL $
 ERRORLEVEL +306

  btfsc   PIR1,RCIF     ; Interrupcion de Rx Serial
  call    RxInt
  ;
  ; dejo esta para lo ultimo, si está habilitada la int entro a ver
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  btfsc   PIE1,TXIE
  call    TxInt
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
; << interrupt code <<

; >>>> Restore context >>>>
  clrf  STATUS              ;bank 0, regardless of current bank,Clears IRP,RP1,RP0
  movf  pclath_saved,w      ;Restore PCLATH
  movwf PCLATH              ;w --> PCLATH
  movf  fsr_saved,w         ;Restore FSR
  movwf FSR                 ;w --> FSR
  swapf status_saved,w      ;Swap status_saved register into W
                            ;(sets bank to original state)
  movwf STATUS              ;w --> STATUS register
  swapf w_saved,f           ;Swap w_saved
  swapf w_saved,w           ;Swap w_saved into W
; <<<< Restore context <<<<

  bsf    INTCON,GIE      ;habilito las interrupciones
  retfie
; ============================================================================
; Zona de TABLAS
; ============================================================================

; ============================================================================
; FcnQ3SetB
; ============================================================================
FcnQ3SetB
  movf    RegTmpG,w
  ; el salto es de a 4 y en cada paso en el offset 0 está la instrucción de
  ; apagado, en el 2 la de encendido y en la 1 y 3 la de return
  addwf   PCL,f
  ;------------- >>
  bcf     INDF,0 ;  << 0 - apagar
  return
  bsf     INDF,0 ;  << 0 - encender
  return
  ;-------------
  bcf     INDF,1 ;  << 1 - apagar
  return
  bsf     INDF,1 ;  << 1 - encender
  return
  ;-------------
  bcf     INDF,2 ;  << 2 - apagar
  return
  bsf     INDF,2 ;  << 2 - encender
  return
  ;-------------
  bcf     INDF,3 ;  << 3 - apagar
  return
  bsf     INDF,3 ;  << 3 - encender
  return
  ;-------------
  bcf     INDF,4 ;  << 4 - apagar
  return
  bsf     INDF,4 ;  << 4 - encender
  return
  ;-------------
  bcf     INDF,5 ;  << 5 - apagar
  return
  bsf     INDF,5 ;  << 5 - encender
  return
  ;-------------
  bcf     INDF,6 ;  << 6 - apagar
  return
  bsf     INDF,6 ;  << 6 - encender
  return
  ;-------------
  bcf     INDF,7 ;  << 7 - apagar
  return
  bsf     INDF,7 ;  << 7 - encender
  return
; ============================================================================
; GetPulseCountS1 - Devuelve la dirección del contador para el bit en w
; ============================================================================
GetPulseCountS1
  addwf   PCL,f
  retlw   ContPulseE1B0
  retlw   ContPulseE1B1
  retlw   ContPulseE1B2
  retlw   ContPulseE1B3
  retlw   ContPulseE1B4
  retlw   ContPulseE1B5
  retlw   ContPulseE1B6
  retlw   ContPulseE1B7
; ============================================================================
; GetPulseCountS2 - Devuelve la dirección del contador para el bit en w
; ============================================================================
GetPulseCountS2
  addwf   PCL,f
  retlw   ContPulseE2B0
  retlw   ContPulseE2B1
  retlw   ContPulseE2B2
  retlw   ContPulseE2B3
  retlw   ContPulseE2B4
  retlw   ContPulseE2B5
  retlw   ContPulseE2B6
  retlw   ContPulseE2B7
; ============================================================================
; SetWBitW - Deja prendido en W solo el bit indicado en W
; ============================================================================
SetWBitW
  addwf   PCL,f
  retlw   0x01
  retlw   0x02
  retlw   0x04
  retlw   0x08
  retlw   0x10
  retlw   0x20
  retlw   0x40
  retlw   0x80
; ============================================================================
; FIN - Zona de TABLAS
  org     0x0100
  include "txrx.inc"
  include "fcn.inc"
  include "eefunc.inc"
; ============================================================================
; ============================================================================
; Tratamiento del vector de RESET (Main)
; ============================================================================
Init
  clrwdt
  clrf    STATUS        ;bank 0  ; Registros de configuracion
  clrf    INTCON
  clrf    PIR1
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  ; (7)    pull-up disabled
  ; (3)    prescaler to WDT
  ; (0..2) prescaler rate 1:128
  movlw   0x8F
  movwf   OPTION_REG
  ; PIE2
  clrf    PIE2
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  ; INTCON
  bsf     INTCON,T0IE     ; interrupcion del TIMER0
  bsf     INTCON,INTE     ; interrupcion externa (RB0)
  bsf     INTCON,PEIE     ; necesario para el serial
  ; TIMER1 por ahora no uso
  clrf    T1CON
  ; Seteo el TIMER2 para transmisión de código infra-rojo
  movlw   0x05    ; Pre=1:4, TMR2ON, Post=1:1
  movwf   T2CON
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movlw   0x7D            ; PR2=125
  movwf   PR2
  bsf     PIE1,TMR2IE      ; Interrupcion del TIMER2
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  ; Inicializacion de perifericos
  clrf    ADCON0
 ERRORLEVEL -302
 ifdef __16F887
  clrf    CCP1CON
  clrf    CCP2CON
  bsf     STATUS,RP0 ; --> BANK1
  movlw   0x80
  movwf   ADCON1
  clrf    WPUB  ; disable pull-ups PORTB
  clrf    IOCB  ; disable PORTB Ints PORTB
  bsf     STATUS,RP1 ; --> BANK3
  clrf    CM1CON0
  clrf    CM2CON0
  clrf    CM2CON1
  clrf    ANSEL
  clrf    ANSELH
  bsf     ANSEL,ANS0
  bsf     ANSEL,ANS1
  bsf     ANSEL,ANS2
  bsf     ANSEL,ANS3
  bcf     STATUS,RP0 ;
  bcf     STATUS,RP1 ; --> BANK0
 else
  bsf     STATUS,RP0 ; --> BANK1
  movlw   0xC2
  movwf   ADCON1
  bcf     STATUS,RP0 ; --> BANK0
 endif
 ERRORLEVEL +302
  ; Inicialización de PORTS
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  ; PORTA: salida solamente RA4
  movlw   0xEF
  movwf   TRISA
  ; PORTB: INT0 como entrada, las demás salida.
  movlw   0x01
  movwf   TRISB
  ; PORTC: no se toca, cada función especial que lo configure
  ; PORTD: para arrancar se deja todo entrada, no se toca
  ; PORTE: todas salidas
  movlw   0x00
  movwf   TRISE
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  ; estado inicial del multiplexor --> nada seleccionado
  bsf     NIN1
  bsf     NIN2
  bcf     OUT1
  bcf     OUT2
  ; estado inicial de lineas de control del display
  bsf     DISPLAY_EN
  bcf     DISPLAY_RS
  bcf     DISPLAY_RW
  ; estado inicial del PORT A y E
  clrf    PORTA
  clrf    PORTE
  ;
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  clrf    ContPulseE1B0
  clrf    ContPulseE1B1
  clrf    ContPulseE1B2
  clrf    ContPulseE1B3
  clrf    ContPulseE1B4
  clrf    ContPulseE1B5
  clrf    ContPulseE1B6
  clrf    ContPulseE1B7
  clrf    ContPulseE2B0
  clrf    ContPulseE2B1
  clrf    ContPulseE2B2
  clrf    ContPulseE2B3
  clrf    ContPulseE2B4
  clrf    ContPulseE2B5
  clrf    ContPulseE2B6
  clrf    ContPulseE2B7
  movlw   0xFF
  movwf   MaskNotifE1
  movwf   MaskNotifE2
  movwf   MaskNotifS1
  movwf   MaskNotifS2
  movwf   MaskNotifAN
  movwf   ANALOG1_MAXH
  movwf   ANALOG1_MAXL
  movwf   ANALOG2_MAXH
  movwf   ANALOG2_MAXL
  movwf   ANALOG3_MAXH
  movwf   ANALOG3_MAXL
  movwf   ANALOG4_MAXH
  movwf   ANALOG4_MAXL
  clrf    ANALOG1_MINH 
  clrf    ANALOG1_MINL 
  clrf    ANALOG2_MINH 
  clrf    ANALOG2_MINL 
  clrf    ANALOG3_MINH 
  clrf    ANALOG3_MINL 
  clrf    ANALOG4_MINH 
  clrf    ANALOG4_MINL
  movlw   0x1F
  movwf   ADALRM1DELAY
  movwf   ADALRM2DELAY
  movwf   ADALRM3DELAY
  movwf   ADALRM4DELAY
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  ; inicializacion de los buffers
  movlw   TXINITBUFFER  ; Inicio del buffer de transmisión
  movwf   TxPtrAdd
  movwf   TxPtrSnd
  movlw   RXINITBUFFER
  movwf   RxPtrAdd
  movwf   RxPtrAddTmp
  movwf   RxPtrRcv
  clrf    RxCharCount
  clrf    RxCharSum
  clrf    RxTimeOut
  clrf    Variant
  clrf    IRCount
  ; Para que notifique todos los estados al ponerse online
  movlw   REINTENTOS
  movwf   CountNotifE1
  movwf   CountNotifE2
  movwf   CountNotifS1
  movwf   CountNotifS2
  movwf   CountNotifA1
  movwf   CountNotifA2
  movwf   CountNotifA3
  movwf   CountNotifA4
  ; Inicializo los divisores del timer
  movlw   DIVISOR1
  movwf   TDIV1
  movlw   DIVISOR2
  movwf   TDIV2
  clrf    ENTRADA1
  clrf    ENTRADA2
  clrf    SALIDA1
  clrf    SALIDA2
  clrf    ANALOG_FLGS
  clrf    DISPLAY_FLGS
  ; inicializo las mascaras de seguidores
  clrf    Mascara_E1S1
  clrf    Mascara_E2S2
  clrf    Mascara_ANS2
  ;
  clrf    DelayDisplay
  clrf    DelayResend
  clrf    ConvertDelay
  clrf    SaveEECount
  clrf    ResetCount
  clrf    CntTrafic
  clrf    RxLastCnt
  clrf    RegTmpD
  clrf    RegTmpL
  ; Default Address
  movlw   DEFADDR
  movwf   LocAddr
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  clrf    LocSeg
  clrf    LocMin
  clrf    LocHora
  clrf    LocFecha
  clrf    LocDia
  clrf    LocMes
  clrf    LocAnio
  clrf    BeepCount
  clrf    BeepRetry
  clrf    BeepDelay
  clrf    DisplayTO
  clrf    BKSALIDA1
  clrf    BKSALIDA2
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  ; Default Baud Rate
  movlw   0x17       ; 9600
  movwf   BaudRate
; =============================================================================
  call    ReadConf        ; LECTURA CONFIGURACION
; =============================================================================
  ; inicialización Serial (despues de leer configuracion por el baudrate)
 ERRORLEVEL -302
  ; Baud Rate
  movf    BaudRate,w
  bsf     STATUS,RP0 ; --> BANK1
  movwf   SPBRG
  ; Tx
  bcf     TRISC,6          ; TX como output
  movlw   0x24             ; Enable TX, 8 Bit, Synch, High baud rate
  movwf   TXSTA
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  ; Rx
  movlw   0x90             ; Enable Rx, 8 bit, Synch, Continuous
  movwf   RCSTA
  bsf     RCSTA,SPEN       ; habilito Rx
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  bsf     PIE1,RCIE        ; Interrupcion de Rx
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  ; Inicialización de flags (despues de leer configuracion)
  clrf    IOFLAGS
  clrf    STATFLAGS
  bsf     CAMBIO_S1
  bsf     CAMBIO_S2
  bcf     CAMBIO_E1
  bcf     CAMBIO_E2
  bcf     CAMBIO_AN
  bcf     DATE_OK           ; para que pida la fecha y hora al host
  bcf     DISPLAY_INIT      ; --- para que haga la inicialización
  bsf     DISPLAY_DATA      ; -/
  ;
  bsf     INTCON,GIE    ; habilitacion global de interrupciones
  ;
  movlw   0x05
  movwf   DelayStart
  call    LongBeep
 ERRORLEVEL -306
  ; Salto a la pagina 1 y llamo a DISPLAY_Refresh para que inicialice el display
  PAGESEL DISPLAY_Refresh
  call    DISPLAY_Refresh
  PAGESEL $
 ERRORLEVEL +306
  goto    Main
; ============================================================================
; Loop del programa
; ============================================================================
Main
  clrwdt
  btfss   SYSREADY
  goto    Main          ; no entro al loop hasta que no está listo
  bcf     RCSTA,CREN    ; reseteo la recepción
  bsf     RCSTA,CREN
  ; para que muestre la Screen0 (si está el display)
  clrf    ScrnId
  bsf     DISPLAY_DATA
MainLoop
  clrwdt
  ; >> Procesamiento interno >>
  call    Presencia
  call    CtrlSeguim
  call    STATUS_Refresh
  call    RxDequeue
  call    TxDequeueCtrl
  call    CheckRxOverflow
 ERRORLEVEL -306
  ; Funciones de la PAGE 1
  PAGESEL IO_Refresh
  call    IO_Refresh
  PAGESEL DISPLAY_Refresh
  call    DISPLAY_Refresh
  PAGESEL CLOCK_Refresh
  call    CLOCK_Refresh
  ; Funciones de la PAGE 2
  PAGESEL Notificaciones
  call    Notificaciones
  ; Vuelvo a la pagina de inicio
  PAGESEL $
 ERRORLEVEL +306
  ; Grabo EE
  btfsc   SAVETOEE 
  call    WriteConf
  ; vuelta a empesar
  goto  MainLoop
; ============================================================================
; STATUS_Refresh
;   mantiene el led de status, el buzzer y controla los botones
;               STAT_LED   <-
;               BUZZER     <-
;               BTN_EN     -> EXT_BOTONES (BANK1)
; ============================================================================
STATUS_Refresh
  ; sondeo los botones
;  bsf     BTN_EN
;  nop
;  movf    DATABUS,w
;  bcf     BTN_EN
;  movwf   EXT_BOTONES
  ; Blink del led de status
STATUS_Led
  bsf     STATUS,RP0 ; --> BANK1
  movf    LocSeg,w
  bcf     STATUS,RP0 ; --> BANK0
  btfss   HSTONLINE
  goto    STATUS_Led_HOff
  andlw   0x01
  btfss   STATUS,Z
  bcf     STAT_LED
  btfsc   STATUS,Z
  bsf     STAT_LED
  ;
  goto    STATUS_Led_End

STATUS_Led_HOff
  andlw   0x04
  btfss   STATUS,Z
  bcf     STAT_LED
  btfsc   STATUS,Z
  bsf     STAT_LED
STATUS_Led_End
  ; FALTA ...



  return
; ============================================================================
; Presencia - Se envia por unica vez un par de segundos luego del arranque
;              si no hay respuesta se reenvia cada 30 segundos
; ============================================================================
Presencia
  ; solamente si está off-line
  btfsc   HSTONLINE
  return
  ; me fijo si tengo que esperar
  movf    DelayResend,f
  btfss   STATUS,Z
  return
  ; Un beep por cada echo offline
  call    ShortBeep
  ; Reintento cada 90 Seg
  movlw   0x5A
  movwf   DelayResend
  ; Inicializo el ensaje
  call    ClrTx
  ; >>>> mensaje de presencia >>>>
  movlw   'Q'        ; start char
  movwf   txtyp
  movlw   HSTADDR    ; dest addr
  movwf   txdst
  movf    LocAddr,w  ; local addr
  movwf   txsrc
  movf    Variant,w  ; contador
  movwf   txcnt
  movlw   'P'        ; comando - 'P' = Aviso de presencia o ping
  movwf   txfcn
  movlw   HARTYPE    ; tipo de dispositivo
  movwf   txdat1
  movlw   HARDVER    ; version de dispositivo
  movwf   txdat2
  movlw   SOFTVER    ; version de software
  movwf   txdat3
  movlw   PROCESSOR_VER ; micro procesador
  movwf   txdat4
  movlw   0             ; presencia de display
  btfsc   DISPLAY_OK
  movlw   0x50 ; display de 80 caracteres
  movwf   txdat5
  ; <<<< mensaje de presencia <<<<
  ; lo encola y termina
  goto    TxEnqueue

; ============================================================================
; CtrlSeguim - SALIDA(n+1) = ( ENTRADA & MASCARA ) | ( SALIDA(n) & -MASCARA )
; ============================================================================
CtrlSeguim
  btfsc   CAMBIO_E1
  call    CtrlSeguim1
  bcf     STATUS,RP0 ; --> BANK0
  btfsc   CAMBIO_E2
  call    CtrlSeguim2
  bcf     STATUS,RP0 ; --> BANK0
  btfsc   CAMBIO_AN
  call    CtrlSeguimAN
  bcf     STATUS,RP0 ; --> BANK0
  return
; ----------------------------------------------------------------------------
CtrlSeguim1
  bcf     CAMBIO_E1
  ; me hago un backup de la salida 1
  movf    SALIDA1,w
  movwf   RegTmpG2
  ; (E & M)
  movf    Mascara_E1S1,w
  andwf   ENTRADA1,w
  movwf   RegTmpG
  ; -M
  movf    Mascara_E1S1,w
  xorlw   0xFF
  ; (Sn & -M)
  andwf   SALIDA1,w
  ; (E & M) | (Sn & -M)
  iorwf   RegTmpG,w
  ; w --> SALIDA1
  movwf   SALIDA1
  bsf     CAMBIO_S1
  return
; ----------------------------------------------------------------------------
CtrlSeguim2
  bcf     CAMBIO_E2
  ; me hago un backup de la salida 2
  movf    SALIDA2,w
  movwf   RegTmpG2
  ; (E & M)
  movf    Mascara_E2S2,w
  andwf   ENTRADA2,w
  movwf   RegTmpG
  ; -M
  movf    Mascara_E2S2,w
  xorlw   0xFF
  ; (Sn & -M)
  andwf   SALIDA2,w
  ; (E & M) | (Sn & -M)
  iorwf   RegTmpG,w
  ; w --> SALIDA2
  movwf   SALIDA2
  bsf     CAMBIO_S2
  return
; ----------------------------------------------------------------------------
CtrlSeguimAN
  bcf     CAMBIO_AN
  ; me hago un backup de la salida 2
  movf    SALIDA2,w
  movwf   RegTmpG2
  ; (AN & M)
  movf    Mascara_ANS2,w
  andwf   ANALOG_FLGS,w
  movwf   RegTmpG
  ; -M
  movf    Mascara_ANS2,w
  xorlw   0xFF
  ; (Sn & -M)
  andwf   SALIDA2,w
  ; (AN & M) | (Sn & -M)
  iorwf   RegTmpG,w
  ; w --> SALIDA2
  movwf   SALIDA2
  bsf     CAMBIO_S2
  return
; ============================================================================
; ShortBeep - inicia un sonido corto del buzzer
; Short2Beep, Short3Beep, Short5Beep - 2, 3 y 5 pitidos
; ============================================================================
ShortBeep
  movlw   0x01
  goto   NumBeep
; -------------------------------------
Short2Beep
  movlw   0x02
  goto   NumBeep
; -------------------------------------
Short3Beep
  movlw   0x03
  goto   NumBeep
; -------------------------------------
Short5Beep
  movlw   0x05
  goto   NumBeep
; ============================================================================
; NumBeep - inicia tantos beeps como indique w
; ============================================================================
NumBeep
  bsf     BUZZER        ; enciendo el buffer
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movwf   BeepRetry     ; me traigo el valor de W y lo aplico a retry
  decf    BeepRetry,f   ; le resto uno
  movlw   SHORT_BEEP    ; seteo la duracion del primer beep
  movwf   BeepCount
  clrf    BeepDelay     ; elimino el delay entre beeps
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  return
; ============================================================================
; LongBeep - inicia un sonido largo del buzzer
; ============================================================================
LongBeep
  bsf     BUZZER
  movlw   LONG_BEEP
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movwf   BeepCount
  clrf    BeepRetry
  clrf    BeepDelay
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  return

; ============================================================================
  END
