// ============================ //
// Do not edit this part!!!!    //
// ============================ //
// 0x300001 - CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator,
                                // PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit
                                // (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit
                                // (Oscillator Switchover mode disabled)
// 0x300002 - CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out
                                // Reset disabled in hardware and software)
// 0x300003 - CONFIG1H
#pragma config WDT = OFF        // Watchdog Timer Enable bit
                                // (WDT disabled (control is placed on the SWDTEN bit))
// 0x300004 - CONFIG3L
// 0x300005 - CONFIG3H
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit
                                // (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled;
                                // RE3 input pin disabled)
// 0x300006 - CONFIG4L
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply
                                // ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit
                                // (Instruction set extension and Indexed
                                // Addressing mode disabled (Legacy mode))
#pragma config DEBUG = OFF      // Disable In-Circuit Debugger

// Timer Related Definitions
#define KHZ 1000UL
#define MHZ (KHZ * KHZ)
#define _XTAL_FREQ (40UL * MHZ)
// ============================ //
//             End              //
// ============================ //
#include <xc.h>
#include <stdint.h>

// ============================ //
//        DEFINITIONS           //
// ============================ //

// You can write struct definitions here...

// ============================ //
//          GLOBALS             //
// ============================ //

// You can write globals definitions here...
uint24_t timer_counter = 0;
uint24_t round_score, total_score;
uint8_t position, size;
uint8_t blink_bit = 0, temp = 0;
uint8_t counter7;
uint8_t interflag, timerflag;
uint8_t D0, D1, D2, D3;

// ============================ //
//          FUNCTIONS           //
// ============================ //

// You can write function definitions here...
void init_ports();
void init_interrupts();
void init_timers();
void start_system();

void jump_hippo();
void blink();
void gravity();
void degrade_score();
void soft_reset();

uint8_t converter(uint8_t x);
// ============================ //
//   INTERRUPT SERVICE ROUTINE  //
// ============================ //
__interrupt(high_priority)
void HandleInterrupt()
{ 
    // ISR ...
    if(INTCONbits.TMR0IF){ // timer interrupt
        INTCONbits.TMR0IF = 0;
        
        TMR0H = 0x3c;
        TMR0L = 0xc3;
        timer_counter++;
        
        switch(counter7++ % 4){
            case 0:
                LATH = 0b00001000;
                LATJ = converter(total_score % 10);
                break;
            case 1:
                LATH = 0b00000100;
                LATJ = converter((total_score / 10)  % 10);
                break;
            case 2:
                LATH = 0b00000010;
                LATJ = converter((total_score / 100) % 10);
                break;
            case 3:
                LATH = 0b00000001;
                LATJ = converter(total_score / 1000);
                break;
        }
        
        timerflag = 1; 
    }
    if(INTCONbits.INT0IF){ // B0 interrupt
        INTCONbits.INT0IF = 0;
        
        interflag = 1;
    }
    
}

// ============================ //
//            MAIN              //
// ============================ //
void main()
{
    // Main ...
    init_ports();

    init_interrupts();
    
    init_timers();
    
    start_system();
    
    while(1){
        if(position == size - 1){
            soft_reset();
        }
        if(interflag){
            interflag = 0;
            jump_hippo();
        }
        if(timerflag){
            timerflag = 0;
            if(timer_counter % 70 == 0)  gravity();                 // 350 ms
            if(timer_counter % 100 == 0) blink();                   // 500 ms
            if(timer_counter % 200 == 0) degrade_score();           // 1 sec
            if(timer_counter == 1400) timer_counter = 0;
        }
    }
}

void init_ports(){
    INTCON = 0;
    T0CON = 0;
    
    TRISB = 0x01;   // B0 for input
    LATB = 0x00;
    
    TRISD = 0x00;   // PORTD for output
    LATD = 0x00;
    
    TRISH = 0x00;   // PORTH for output
    LATH = 0x00;
    
    TRISJ = 0x00;   // PORTJ for output
    LATJ = 0x00;
}

void init_interrupts(){
    INTCONbits.INT0IE = 1;
    
}

void init_timers(){
    INTCONbits.TMR0IE = 1;
    T0CONbits.PSA = 1;
    TMR0H = 0x3c;
    TMR0L = 0xc3;
}

void start_system(){
    LATJ = 0b00111111;
    __delay_ms(1000);
    INTCONbits.GIE = 1; // enable interrupts
    T0CONbits.TMR0ON = 1; // start timer
    LATDbits.LATD0 = 1; // start the blink
    LATDbits.LATD7 = 1; // initialize hippo
    position = 7;
    size = 1;
    round_score = 100;
    total_score = 0;
    interflag = timerflag = 0;
    counter7 = 0;
}

void jump_hippo(){
    switch(size){
        case 1:
            switch(position){
                case 7:
                    LATDbits.LATD7 = 0;
                    LATDbits.LATD6 = 1;
                    break;
                case 6:
                    LATDbits.LATD6 = 0;
                    LATDbits.LATD5 = 1;
                    break;
                case 5:
                    LATDbits.LATD5 = 0;
                    LATDbits.LATD4 = 1;
                    break;
                case 4:
                    LATDbits.LATD4 = 0;
                    LATDbits.LATD3 = 1;
                    break;
                case 3:
                    LATDbits.LATD3 = 0;
                    LATDbits.LATD2 = 1;
                    break;
                case 2:
                    LATDbits.LATD2 = 0;
                    LATDbits.LATD1 = 1;
                    break;
                default:
                    break;
            }
            break;
        case 2:
            switch(position){
                case 7:
                    LATDbits.LATD7 = 0;
                    LATDbits.LATD5 = 1;
                    break;
                case 6:
                    LATDbits.LATD6 = 0;
                    LATDbits.LATD4 = 1;
                    break;
                case 5:
                    LATDbits.LATD5 = 0;
                    LATDbits.LATD3 = 1;
                    break;
                case 4:
                    LATDbits.LATD4 = 0;
                    LATDbits.LATD2 = 1;
                    break;
                case 3:
                    LATDbits.LATD3 = 0;
                    LATDbits.LATD1 = 1;
                    break;
                default:
                    break;
            }
            break;
        case 3:
            switch(position){
                case 7:
                    LATDbits.LATD7 = 0;
                    LATDbits.LATD4 = 1;
                    break;
                case 6:
                    LATDbits.LATD6 = 0;
                    LATDbits.LATD3 = 1;
                    break;
                case 5:
                    LATDbits.LATD5 = 0;
                    LATDbits.LATD2 = 1;
                    break;
                case 4:
                    LATDbits.LATD4 = 0;
                    LATDbits.LATD1 = 1;
                    break;
                default:
                    break;
            }
            break;
        case 4:
            switch(position){
                case 7:
                    LATDbits.LATD7 = 0;
                    LATDbits.LATD3 = 1;
                    break;
                case 6:
                    LATDbits.LATD6 = 0;
                    LATDbits.LATD2 = 1;
                    break;
                case 5:
                    LATDbits.LATD5 = 0;
                    LATDbits.LATD1 = 1;
                    break;
                default:
                    break;
            }
            break;
        case 5:
            switch(position){
                case 7:
                    LATDbits.LATD7 = 0;
                    LATDbits.LATD2 = 1;
                    break;
                case 6:
                    LATDbits.LATD6 = 0;
                    LATDbits.LATD1 = 1;
                    break;
                default:
                    break;
            }
            break;   
    }
    if(position) position--;
}
void blink(){
    LATDbits.LATD0  = PORTDbits.RD0 ^ 1; // TOGGLE
}
void gravity(){
    switch(size){
        case 1:
            switch(position){
                case 1:
                    LATDbits.LATD1 = 0;
                    LATDbits.LATD2 = 1;
                    break;
                case 2:
                    LATDbits.LATD2 = 0;
                    LATDbits.LATD3 = 1;
                    break;
                case 3:
                    LATDbits.LATD3 = 0;
                    LATDbits.LATD4 = 1;
                    break;
                case 4:
                    LATDbits.LATD4 = 0;
                    LATDbits.LATD5 = 1;
                    break;
                case 5:
                    LATDbits.LATD5 = 0;
                    LATDbits.LATD6 = 1;
                    break;
                case 6:
                    LATDbits.LATD6 = 0;
                    LATDbits.LATD7 = 1;
                    break;
                default:
                    break;
            }
            break;
        case 2:
            switch(position){
                case 2:
                    LATDbits.LATD1 = 0;
                    LATDbits.LATD3 = 1;
                    break;
                case 3:
                    LATDbits.LATD2 = 0;
                    LATDbits.LATD4 = 1;
                    break;
                case 4:
                    LATDbits.LATD3 = 0;
                    LATDbits.LATD5 = 1;
                    break;
                case 5:
                    LATDbits.LATD4 = 0;
                    LATDbits.LATD6 = 1;
                    break;
                case 6:
                    LATDbits.LATD5 = 0;
                    LATDbits.LATD7 = 1;
                    break;
                default:
                    break;
            }
            break;
        case 3:
            switch(position){
                case 3:
                    LATDbits.LATD1 = 0;
                    LATDbits.LATD4 = 1;
                    break;
                case 4:
                    LATDbits.LATD2 = 0;
                    LATDbits.LATD5 = 1;
                    break;
                case 5:
                    LATDbits.LATD3 = 0;
                    LATDbits.LATD6 = 1;
                    break;
                case 6:
                    LATDbits.LATD4 = 0;
                    LATDbits.LATD7 = 1;
                    break;
                default:
                    break;
            }
            break;
        case 4:
            switch(position){
                case 4:
                    LATDbits.LATD1 = 0;
                    LATDbits.LATD5 = 1;
                    break;
                case 5:
                    LATDbits.LATD2 = 0;
                    LATDbits.LATD6 = 1;
                    break;
                case 6:
                    LATDbits.LATD3 = 0;
                    LATDbits.LATD7 = 1;
                    break;
                default:
                    break;
            }
            break;
        case 5:
            switch(position){
                case 5:
                    LATDbits.LATD1 = 0;
                    LATDbits.LATD6 = 1;
                    break;
                case 6:
                    LATDbits.LATD2 = 0;
                    LATDbits.LATD7 = 1;
                    break;
                default:
                    break;
            }
            break;       
    }
    
    if(position < 7) position++;
}

void degrade_score(){
    if(round_score > 0) round_score -= 10;
}

void soft_reset(){
    total_score += round_score;
    round_score = 100;
    //lit all LEDS
    LATD = 0xff;
    __delay_ms(400);
    LATD = 0x00;
    __delay_ms(400);
    LATD = 0xff;
    __delay_ms(400);
    LATD = 0x00;
    __delay_ms(400);
    LATD = 0xff;
    __delay_ms(400);
    // Initialize new start
    size++;
    position = 7;
    if(size == 6) size = 1; //hard reset
      
    switch(size){
        case 1:
            LATD = 0b10000001;
            break;
        case 2:
            LATD = 0b11000001;
            break;
        case 3:
            LATD = 0b11100001;
            break;
        case 4:
            LATD = 0b11110001;
            break;
        case 5:
            LATD = 0b11111001;
            break;
        default:
            break;
    }
            
    
    // TIMER RESET
    TMR0H = 0x3c;
    TMR0L = 0xc3;
    timer_counter = 0;
    interflag = 0;
    timerflag = 0;
    
}

uint8_t converter(uint8_t x){           // Convert a number to the 7-segment display format
    switch(x){
        case 0:
            return 0b00111111;
        case 1:
            return 0b00000110;
        case 2:
            return 0b01011011;
        case 3:
            return 0b01001111;
        case 4:
            return 0b01100110;
        case 5:
            return 0b01101101;
        case 6:
            return 0b01111101;
        case 7:
            return 0b00000111;
        case 8:
            return 0b01111111;
        case 9:
            return 0b01101111;
            
        default:
            break;
    }
}
