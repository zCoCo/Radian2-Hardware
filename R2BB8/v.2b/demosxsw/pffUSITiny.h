#ifndef pffUSITiny_h
#define pffUSITiny_h
#include <avr/io.h>
#include <util/delay_basic.h>
#include "integer.h"
#include <Arduino.h>
#include "t1634_Pinout.h"

// SD chip select pin
#define SD_CS_PIN PB0
  #define SD_CS_PORT PORTB
  #define SD_CS_DDR DDRB
  #define SD_CS_BIT 0

// Use SPI SCK divisor of 2 if nonzero else 4.
#define SPI_FCPU_DIV_2 1
//------------------------------------------------------------------------------
#define	FORWARD(d)	xmit(d)				/* Data forwarding function (console out) */
static void xmit(char d) {}  // Dummy write console
//------------------------------------------------------------------------------
/** Send a byte to the card */
inline void xmit_spi(BYTE d) {
  USIDR = d; USISR = _BV(USIOIF); while ( !(USISR & _BV(USIOIF)) ) USICR |= _BV(USITC);
}
//------------------------------------------------------------------------------
/** Receive a byte from the card */
inline BYTE rcv_spi (void) {xmit_spi(0XFF); return USIDR;}
//------------------------------------------------------------------------------
// Use standard pin functions on other AVR boards.

#if (defined(__AVR_ATtiny1634__)) //SDCS = PB0 on R2BB8
  #define SD_CS_MASK (1 << SD_CS_BIT)
  #define SELECT()  (SD_CS_PORT &= ~SD_CS_MASK)	 /* CS = L */
  #define	DESELECT()	(SD_CS_PORT |= SD_CS_MASK)	/* CS = H */
  #define	SELECTING	!(SD_CS_PORT & SD_CS_MASK)	  /* CS status (true:CS low) */
#else
  #define SELECT() digitalWrite(SD_CS_PIN, LOW)
  #define DESELECT() digitalWrite(SD_CS_PIN, HIGH)
  #define SELECTING !digitalRead(SD_CS_PIN)
#endif //MCU Architecture

static void init_spi (void) {
  digitalWrite(SS, HIGH);
  pinMode(SS, OUTPUT);
  pinMode(USI_DO, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(SD_CS_PIN, OUTPUT);
}
//------------------------------------------------------------------------------
static void dly_100us (void) {
  // each count delays four CPU cycles.
  _delay_loop_2(F_CPU/(40000));
}
#endif  // pffUSITiny_h
