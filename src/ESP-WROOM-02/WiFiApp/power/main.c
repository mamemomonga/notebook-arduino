#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

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
#define BT1       ( 1<<PB1 )
#define BT1_INPU  { DDRB &=~ BT1; PORTB |= BT1; }
#define BT1_IS_H  ( PINB &   BT1 )
#define BT1_IS_L !( PINB &   BT1 )

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

void do_stop();
void do_start();
void do_program();
void do_power_toggle();
void init();
void loop();
int main(void);

uint8_t running=0;
uint8_t program=0;
uint8_t dtr_detect=0;

int main(void) {
	init();
    for(;;){
		loop();
    }
    return 0;
}

void init() {
	EN_OUT;
	DTR_IN;
	BT1_INPU;
	BT2_INPU;
	IO0_INPU;

	// INT0割り込み無効
	GIMSK &=~ INT0;

	// DTRがHならDTR検出モード
	if(DTR_IS_H) {
		dtr_detect=1;
	}

	do_start();
}

void loop() {
	// ボタン1
	if(BT1_IS_L) {
		// プログラム直後はリセット
		if(program) {
			do_start();
		// 電源オンのときはオフ
		} else if (running) {
			do_power_toggle();
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

void do_stop() {
	running=0;
	program=0;
	EN_L;
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

void do_power_toggle() {
	do_stop();
	_delay_ms(1000);

	// Deep Sleep
	// INT0割り込みを有効
	GIMSK |= (1<<INT0);

	// INT0 がLowで割り込み
	MCUCR &=~ (1<<ISC01);
	MCUCR &=~ (1<<ISC00);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sei();
	sleep_enable();
	sleep_cpu();

	// INT0割り込み無効
	GIMSK &=~ (1<<INT0);

	do_start();
}


