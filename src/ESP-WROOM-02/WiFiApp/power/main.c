#include <avr/io.h>
#include <util/delay.h>

/*
 ATtiny13A 8PDIP

/RST | 1    8 | VCC
 PB3 | 2    7 | PB2
 PB4 | 3    6 | PB1
 GND | 4    5 | PB0
*/

// オレンジ 
#define BT2       ( 1<<PB0 )
#define BT2_INPU  { DDRB &=~ BT2; PORTB |= BT2; }
#define BT2_IS_H  ( PINB &   BT2 )
#define BT2_IS_L !( PINB &   BT2 )

// 赤 INT0
#define BT2       ( 1<<PB1 )
#define BT2_INPU  { DDRB &=~ BT1; PORTB |= BT1; }
#define BT2_IS_H  ( PINB &   BT1 )
#define BT2_IS_L !( PINB &   BT1 )

// ESPプログラム用
// USB-UART DTRピン
#define DTR       ( 1<<PB2 )
#define DTR_OUT     DDRB  |=  DTR 
#define DTR_IN    { DDRB  &=~ DTR; PORTB &=~ DTR; }
#define DTR_INPU  { DDRB  &=~ DTR; PORTB |=  DTR; }
#define DTR_H       PORTB |=  DTR 
#define DTR_L       PORTB &=~ DTR 
#define DTR_IS_H  ( PINB  &   DTR )
#define DTR_IS_L !( PINB  &   DTR )

// ESP ENピン
#define EN      ( 1<<PB3 )
#define EN_OUT  DDRB  |=  EN 
#define EN_H    PORTB |=  EN 
#define EN_L    PORTB &=~ EN 
#define EN_I    PORTB ^=  EN 

// ESP IO0ピン
#define IO0       ( 1<<PB4 )
#define IO0_OUT     DDRB |=   IO0 
#define IO0_IN    { DDRB &=~  IO0; PORTB &=~ IO0; }
#define IO0_INPU  { DDRB &=~  IO0; PORTB |=  IO0; }
#define IO0_H       PORTB |=  IO0 
#define IO0_L       PORTB &=~ IO0 
#define IO0_IS_H  ( PINB &    IO0 )
#define IO0_IS_L !( PINB &    IO0 )

uint8_t running=0;
uint8_t program=0;
uint8_t dtr_detect=0;

void do_stop() {
	running=0;
	program=0;
	EN_L;
	_delay_ms(1000);
	IO0_INPU;
}

void do_start() {
	running=1;
	program=0;
	EN_L;
	IO0_OUT;
	_delay_ms(100);
	EN_H;
	_delay_ms(1000);
	IO0_INPU;
}

void do_program() {
	running=0;
	program=1;
	EN_L;
	IO0_OUT;
	IO0_L;
	_delay_ms(100);
	EN_H;
	_delay_ms(100);
	IO0_H;
	_delay_ms(1000);
	IO0_INPU;
}

int main(void) {

	EN_OUT;
	DTR_IN;
	BT1_INPU;
	BT2_INPU;
	IO0_INPU;

	// INT0割り込み無効
	GIMSK &=~ INT0;

	do_stop();

	// DTRがHならDTR検出モード
	if(DTR_IS_H) {
		dtr_detect=1;
	}

    for(;;){
		// ボタン1
		if(BT1_IS_L) {
			// プログラム直後はリセット
			if(program) {
				do_start();
			// 電源オンのときはオフ
			} else if (running) {
				do_stop();
			// 電源オフのときはオン
			} else {
				do_start();
			}
		}
		// ボタン2
		if(BT2_IS_L) {
		}

		// 動作中
		if(running) {
			// IO0がLになってHになったらリセット
			if(IO0_IS_L) {
				do_stop();
				while(IO0_IS_L) {
					_delay_ms(1);
				}
				do_start();
			}
		}
		// DTRがLになったらプログラムモード
		if(dtr_detect) {
			if(DTR_IS_L) {
				do_program();
			}
		}
    }
    return 0;
}
