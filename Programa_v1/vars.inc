; ============================================================================
; Variables
; ============================================================================
; BANK-0 (0x20..0x6F)
; Salvado de Contexto (el de W está en el área global)
status_saved  equ 0x20  ; para el salvado de contexto (resgistro STATUS)
pclath_saved  equ 0x21  ; para el salvado de contexto (registro PCLATH)
fsr_saved     equ 0x22  ; para el salvado de contexto (registro FSR)
; Flags de I/O
TDIV1         equ 0x23  ; Contador para el divisor del timer
TDIV2         equ 0x24  ; Contador para el divisor del timer
;
DelayDisplay  equ 0x25  ; Para implementar un delay entre cambios de mensajes 
ErrCde1       equ 0x26  ; almacena el numero del ultimo error el procesar un mensaje
DisplayTO     equ 0x27  ; Lleva la cuenta del time-out en la inicializacion del
                        ; display para determinar si está conectado
Variant       equ 0x28  ; secuencia para los requerimientos salientes
;
TxPtrAdd      equ 0x29  ; Puntero donde agregar datos para enviar en BANK2
TxPtrSnd      equ 0x2A  ; puntero donde leer datos para enviar en BANK2
RxPtrAdd      equ 0x2B  ; puntero donde agregar datos recibidos en BANK3 se le
                        ; asigna el TMP cuando tengo un mensaje OK
RxPtrAddTmp   equ 0x2C  ; puntero temporal donde agregar datos recibidos en
                        ; BANK3 para volver en caso de error
RxPtrRcv      equ 0x2D  ; puntaro donde leer para procesar datos recibidos
                        ; en BANK3
RxChar        equ 0x2E  ; acá se mueve el caracter recibido para tratarlo
RxCharCount   equ 0x2F  ; lleva la cuenta de los caracteres recibidos para un
                        ; mensaje
RxCharSum     equ 0x30  ; lleva el calculo del checksum a medida que llega el
                        ; mensaje
RxTimeOut     equ 0x31  ; Es para dar un timeout a los mensajes incompletos
RxLastCnt     equ 0x32  ; Mantiene el valor de ultimo contador de msg recibido
SaveEECount   equ 0x33  ; Cuenta regresiva para grabar configuracion
ResetCount    equ 0x34  ; Delay para reset por soft
DelayStart    equ 0x35  ; Delay de aranque
DelayResend   equ 0x36  ; Contador de Delay para cualquier reenvío de mensaje
CountNotifE1  equ 0x37  ; Reintentos de reenvio de notificacion de E1
CountNotifE2  equ 0x38  ; Reintentos de reenvio de notificacion de E2
CountNotifA1  equ 0x39  ; Reintentos de reenvio de notificacion de AN1
CountNotifA2  equ 0x3A  ; Reintentos de reenvio de notificacion de AN2
CountNotifA3  equ 0x3B  ; Reintentos de reenvio de notificacion de AN3
CountNotifA4  equ 0x3C  ; Reintentos de reenvio de notificacion de AN4
CountNotifS1  equ 0x3D  ; Reintentos de reenvio de notificacion de S1
CountNotifS2  equ 0x3E  ; Reintentos de reenvio de notificacion de S2
ANALOG1H      equ 0x3F  ; imagen interna de entrada analogica 1 - parte alta
ANALOG1L      equ 0x40  ; imagen interna de entrada analogica 1 - parte baja
ANALOG2H      equ 0x41  ; imagen interna de entrada analogica 2 - parte alta
ANALOG2L      equ 0x42  ; imagen interna de entrada analogica 2 - parte baja
ANALOG3H      equ 0x43  ; imagen interna de entrada analogica 3 - parte alta
ANALOG3L      equ 0x44  ; imagen interna de entrada analogica 3 - parte baja
ANALOG4H      equ 0x45  ; imagen interna de entrada analogica 4 - parte alta
ANALOG4L      equ 0x46  ; imagen interna de entrada analogica 4 - parte baja
ConvertDelay  equ 0x47  ; espera luego de elegir una entrada A/D
CntTrafic     equ 0x48  ; Contador para WFT
RegTmpI       equ 0x49  ; Registro temporal para usar dentro de interrupciones
RegTmpG       equ 0x4A  ; registro temporal generico
RegTmpG2      equ 0x4B  ; registro temporal generico
RegTmpD       equ 0x4C  ; Registro temporal para funciones de delay
RegTmpL       equ 0x4D  ; Registro temporal para funciones de delay (cuando hay loop de delay)
ReadIRetry    equ 0x4E  ; contador para los reintentos de lectura de entrada
IRCount       equ 0x4F
; - Registro para colocar un mensaje salido del buffer de entrada y procesarlo
; - RX
rxtyp         equ 0x50 ; -----
rxdst         equ 0x51 ;      |
rxsrc         equ 0x52 ;      |
rxcnt         equ 0x53 ;      |
rxfcn         equ 0x54 ;      |
rxdat1        equ 0x55 ;      |
rxdat2        equ 0x56 ;      |
rxdat3        equ 0x57 ;      | Para referencia directa
rxdat4        equ 0x58 ;      |
rxdat5        equ 0x59 ;      |
rxdat6        equ 0x5A ;      |
rxdat7        equ 0x5B ;      |
rxdat8        equ 0x5C ;      |
rxdat9        equ 0x5D ;      |
rxdat10       equ 0x5E ;      |
rxchksum      equ 0x5F ; -----
;
; - Registro para el armado de mensaje saliente antes de transferir al buffer
;   de salida.
; - TX
txtyp         equ 0x60 ; -----
txdst         equ 0x61 ;      |
txsrc         equ 0x62 ;      |
txcnt         equ 0x63 ;      |
txfcn         equ 0x64 ;      |
txdat1        equ 0x65 ;      |
txdat2        equ 0x66 ;      |
txdat3        equ 0x67 ;      | Para referencia directa
txdat4        equ 0x68 ;      |
txdat5        equ 0x69 ;      |
txdat6        equ 0x6A ;      |
txdat7        equ 0x6B ;      |
txdat8        equ 0x6C ;      |
txdat9        equ 0x6D ;      |
txdat10       equ 0x6E ;      |
txchksum      equ 0x6F ; -----

; ============================================================================
; BANK-1 (0xA0..0xEF)
ContBtn0      equ 0xA0 ;---
ContBtn1      equ 0xA1 ;   |
ContBtn2      equ 0xA2 ;   | Filtro de ruido para botones
ContBtn3      equ 0xA3 ;   | 0x00      = boton suelto
ContBtn4      equ 0xA4 ;   | BTNFILTER = boton apretado
ContBtn5      equ 0xA5 ;   |
ContBtn6      equ 0xA6 ;   |
ContBtn7      equ 0xA7 ;---
BOTONES       equ 0xA8 ; estado filtrado de los botones
EXT_BOTONES   equ 0xA9 ; estado crudo de los botones
HEXADECIMAL   equ 0xAA
DEC_H         equ 0xAB
DEC_M         equ 0xAC
DEC_L         equ 0xAD
BeepRetry     equ 0xAE
BeepDelay     equ 0xAF
BeepCount     equ 0xB0
BKSALIDA1     equ 0xB1
BKSALIDA2     equ 0xB2
ADALRM1DELAY  equ 0xB3
ADALRM2DELAY  equ 0xB4
ADALRM3DELAY  equ 0xB5
ADALRM4DELAY  equ 0xB6
IRCodeByte    equ 0xB7 ; Se carga con el byte IR que se está transmitiendo
IRCodeByteI   equ 0xB8 ; Contiene el indice del byte IR que se debe cargar
IRCode1       equ 0xB9
IRCode2       equ 0xBA
IRCode3       equ 0xBB
IRCode4       equ 0xBC
IRCode5       equ 0xBD
;             equ 0xBE
;             equ 0xBF
; ----------------------------------------------------------------------------
; 0xC0 a 0xEF - Datos persistentes fuera del área global (Colita del BANK-1)
; ----------------------------------------------------------------------------
MaskNotifE1   equ 0xC0 ;---
MaskNotifE2   equ 0xC1 ;   |
MaskNotifS1   equ 0xC2 ;   | Mascaras para notificacion de cambios
MaskNotifS2   equ 0xC3 ;   |
MaskNotifAN   equ 0xC4 ;---
LocHora       equ 0xC5
LocMin        equ 0xC6
LocSeg        equ 0xC7
LocDia        equ 0xC8
LocFecha      equ 0xC9
LocMes        equ 0xCA
LocAnio       equ 0xCB
;             equ 0xCC
;             equ 0xCD
;             equ 0xCE
;             equ 0xCF
ContPulseE1B0 equ 0xD0 ;---
ContPulseE1B1 equ 0xD1 ;   |
ContPulseE1B2 equ 0xD2 ;   |
ContPulseE1B3 equ 0xD3 ;   |
ContPulseE1B4 equ 0xD4 ;   |
ContPulseE1B5 equ 0xD5 ;   |
ContPulseE1B6 equ 0xD6 ;   |
ContPulseE1B7 equ 0xD7 ;   | Contadores para el regreso del pulso
ContPulseE2B0 equ 0xD8 ;   |
ContPulseE2B1 equ 0xD9 ;   |
ContPulseE2B2 equ 0xDA ;   |
ContPulseE2B3 equ 0xDB ;   |
ContPulseE2B4 equ 0xDC ;   |
ContPulseE2B5 equ 0xDD ;   |
ContPulseE2B6 equ 0xDE ;   |
ContPulseE2B7 equ 0xDF ;--- 
ANALOG1_MAXH  equ 0xE0
ANALOG1_MAXL  equ 0xE1
ANALOG1_MINH  equ 0xE2
ANALOG1_MINL  equ 0xE3
ANALOG2_MAXH  equ 0xE4
ANALOG2_MAXL  equ 0xE5
ANALOG2_MINH  equ 0xE6
ANALOG2_MINL  equ 0xE7
ANALOG3_MAXH  equ 0xE8
ANALOG3_MAXL  equ 0xE9
ANALOG3_MINH  equ 0xEA
ANALOG3_MINL  equ 0xEB  
ANALOG4_MAXH  equ 0xEC
ANALOG4_MAXL  equ 0xED
ANALOG4_MINH  equ 0xEE
ANALOG4_MINL  equ 0xEF

; ============================================================================
; BANK-2 (0x110..0x16F)
TXINITBUFFER  equ 0x10    ; inicio de cola de transmisión (16 Bytes por msg)
TXMAXBUFFER   equ 0x6F    ; limite superior de la cola de transmisión

; ============================================================================
; BANK-3 (0x190..0x1EF)
; buffer de recepción
RXINITBUFFER  equ 0x90    ; inicio de cola de recepción (15 bytes por msg)
RXMAXBUFFER   equ 0xE9    ; limite superior de la cola de recepción

; ============================================================================
; Globales (0x70..0x7F) (0xF0..0xFF) (0x170..0x17F) (0x1F0..0x1FF)
; backup de contexto en INT
w_saved       equ 0x70 ; para el salvado de contexto (registro w)
; Configuracion y estados perdurables
LocAddr       equ 0x71 ; Dirección local
SALIDA1       equ 0x72
SALIDA2       equ 0x73
ENTRADA1      equ 0x74  ; imagen interna de entrada 1
ENTRADA2      equ 0x75  ; imagen interna de entrada 2
Mascara_E1S1  equ 0x76  ; Configuracion bit a bit del seguimiento a las entradas 1 por las salidas 1
Mascara_E2S2  equ 0x77  ; Configuracion bit a bit del seguimiento a las entradas 2 por las salidas 2
Mascara_ANS2  equ 0x78  ; Configuracion bit a bit del seguimiento a las entradas analogicas por las salidas 2
ANALOG_FLGS   equ 0x79  ; flags de estados de entradas analogicas
#define AN1_UMBRAL ANALOG_FLGS,0    ; 0 = (ANALOG1 <= ANALOG1_MIN)
                                    ; 1 = (ANALOG1 >= ANALOG1_MAX)
#define AN2_UMBRAL ANALOG_FLGS,1    ;
#define AN3_UMBRAL ANALOG_FLGS,2    ;
#define AN4_UMBRAL ANALOG_FLGS,3    ;
#define ADC_READY  ANALOG_FLGS,4    ; Indica que ya fueron escaneados por lo
                                    ; menos una vez todas las entradas
; Serial Baud Rate
;  0xBF      1200 
;  0x5F      2400
;  0x17      9600 *
;  0x0B     19200
BaudRate      equ 0x7A
DISPLAY_FLGS  equ 0x7B
#define DISPLAY_OK    DISPLAY_FLGS,0  ; Indica la presencia del display
#define DISPLAY_INIT  DISPLAY_FLGS,1  ; Indica que el display ya está inicializado
#define DISPLAY_DATA  DISPLAY_FLGS,2  ; Indica que hay datos para mostrar en el display
#define DATE_OK       DISPLAY_FLGS,3  ; Indica que ya se obtuvo una fecha y hora valida
#define DISPLAY_CLOCK DISPLAY_FLGS,4  ; Indica que hay que refrescar el reloj

ScrnId        equ 0x7C
;             equ 0x7D
; Registros que se acceden de varios bancos
IOFLAGS       equ 0x7E
#define CAMBIO_E1 IOFLAGS,0  ;indica cambio en el estado de las entradas o salidas
#define CAMBIO_E2 IOFLAGS,1
#define CAMBIO_S1 IOFLAGS,2
#define CAMBIO_S2 IOFLAGS,3
#define CAMBIO_AN IOFLAGS,4
#define SEND_IR   IOFLAGS,5  ;indica que se debe transmitir un código infrarrojo
STATFLAGS     equ 0x7F
#define HSTONLINE STATFLAGS,0  ; indica que el host está on-line
#define SYSREADY  STATFLAGS,1  ; se pone a 1 un par de segundos despues del start
#define TXBUFFULL STATFLAGS,2
#define RXBUFFULL STATFLAGS,3
#define RXCHARERR STATFLAGS,4  ; se utiliza en el control de cada caracter recibido para
                               ; indicar si sirve o no
#define RXCHAREND STATFLAGS,5  ; se utiliza para indicar que se terminó de recibir
                               ; un mensaje
#define SAVETOEE  STATFLAGS,6  ; indica que en el proximo ciclo del loop se graba EE
#define DUPMSG    STATFLAGS,7  ; indica que el mensaje recibido está duplicado
