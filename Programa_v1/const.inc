; ============================================================================
; Constantes
; ============================================================================
#define HARTYPE       0x02    ; tipo de placa
#define HARDVER       0x01    ; versión de hard
#define SOFTVER       0x01    ; version de soft

 ifdef __16F877
#define PROCESSOR_VER 0x01    ; procesador 16F877
 endif
 ifdef __16F877A
#define PROCESSOR_VER 0x02    ; procesador 16F877A
 endif
 ifdef __16F887
#define PROCESSOR_VER 0x03    ; procesador 16F887
 endif

#define DEFADDR       0xFE    ; direccion inicial de RS-232 de la placa
#define HSTADDR       0x00    ; direccion inicial de RS-232 del host
#define BSTADDR       0xFF    ; direccion RS-232 de broadcast
#define MARCA_EE_OK   0xB8    ; Marca que se escibe al inicio de la EE, modificar
                              ; cada vez que se cambie su estructura
                              ; se debe saltear FF Y 00
#define SENS_INPUT    0x05    ; cantidad de veces que se sondea una entrada
#define CONFIG_START  0xC0    ; Dirección de inicio del área de configuracion
#define CONV_DELAY    0x05    ; Delay entre la selección del canal y el GO (x10ms)
#define DIVISOR1      0x24    ; divisor del timer para 10ms aprox
#define DIVISOR2      0x64    ; divisor del timer para 1s aprox
#define BTNFILTER     0x32    ; delay para deteccion de botón (x10ms)
                              ; BANK3)
#define REINTENTOS    0x0A    ; reintentos de envio de un requerimiento antes de
                              ; descartarlo
#define REINT_DELAY   0x0C    ; delay maximo entre reintentos
#define LONG_BEEP     0x32
#define SHORT_BEEP    0x06
#define BEEP_DELAY    0x0C
; Funcionalidad de bits del PORTA
#define IR_OUT        PORTA,4


; Funcionalidad de bits del PORTB
#define INT0          PORTB,0
#define DISPLAY_EN    PORTB,1
#define DISPLAY_RS    PORTB,2
#define DISPLAY_RW    PORTB,3
#define NIN1          PORTB,4
#define OUT2          PORTB,5
#define OUT1          PORTB,6
#define NIN2          PORTB,7
; Bus de datos PORTD
#define DATABUS       PORTD
#define TRISBUS       TRISD
; Funcionalidad de bits del PORTE
#define STAT_LED      PORTE,0      ; led de status (on board o en panel)
#define BUZZER        PORTE,1      ; buzzer (on board o en panel)
#define BTN_EN        PORTE,2      ; linea de habilitación de botones de teclado
