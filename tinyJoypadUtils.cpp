// This file works for TinyJoypad compatible devices.
//
// If not compiled for ATTiny85 (meaning __AVR_ATtiny85__ is not defined),
// generic functions are used instead of direct port access, which
// makes it possible to use an Arduino or Mega2560 (or many others)
// for debugging with serial output or even hardware breakpoints.
//

#include <Arduino.h>
#include "tinyJoypadUtils.h"

#if defined(__AVR_ATtiny85__)
  #include "FastTinyDriver.h"
#else
  #include "SerialHexTools.h"
  #include <Adafruit_SSD1306.h>
  Adafruit_SSD1306 display( 128, 64, &Wire, -1 );
  uint8_t *adafruitBuffer;
#endif

/*-------------------------------------------------------*/
void InitTinyJoypad()
{
#if defined(__AVR_ATtiny85__)
  DDRB &= ~( ( 1 << PB5) | ( 1 << PB3 ) | ( 1 << PB1 ) );
  DDRB |= ( 1 << PB4 );
#else
  pinMode( LEFT_RIGHT_BUTTON, INPUT );
  pinMode( UP_DOWN_BUTTON, INPUT );
  pinMode( FIRE_BUTTON, INPUT );
  DDRB |= ( 1 << PB4 );
  Serial.begin( 115200 );
#endif
}

bool isLeftPressed()
{
  uint16_t inputX = analogRead( LEFT_RIGHT_BUTTON );
  return( ( inputX >= 750 ) && ( inputX < 950 ) );
}
bool isRightPressed()
{
  uint16_t inputX = analogRead( LEFT_RIGHT_BUTTON );
  return( ( inputX > 500 ) && ( inputX < 750 ) );
}
bool isUpPressed()
{
  uint16_t inputY = analogRead( UP_DOWN_BUTTON );
  return( ( inputY > 500 ) && ( inputY < 750 ) );
}
bool isDownPressed()
{
  uint16_t inputY = analogRead( UP_DOWN_BUTTON );
  return( ( inputY >= 750 ) && ( inputY < 950 ) );
}
bool isFirePressed()
{
  return( digitalRead( FIRE_BUTTON ) == 0 );
}
void waitUntilButtonsReleased()
{
  while( isLeftPressed() || isRightPressed() || isUpPressed() || isDownPressed() || isFirePressed() );
}
void waitUntilButtonsReleased( const uint8_t delay )
{
  waitUntilButtonsReleased();
  _delay_ms( delay );
}

void _variableDelay_us( uint8_t delayValue ) {
  while ( delayValue-- != 0 ) { _delay_us( 1 ); }
}

void Sound( const uint8_t freq, const uint8_t dur )
{
  for ( uint8_t t = 0; t < dur; t++ )
  {
    if ( freq!=0 ){ PORTB = PORTB|0b00010000; }
    _variableDelay_us( 255 - freq );
    PORTB = PORTB&0b11101111;
    _variableDelay_us( 255 - freq );
  }
}

/*-------------------------------------------------------*/
void InitDisplay()
{
#if defined(__AVR_ATtiny85__)
  TinyOLED_init();
  for (uint8_t y = 0; y < 8; y++) {
    ssd1306_selectPage(y);
    for (uint8_t x = 0; x < 128; x++) i2c_write(0x00);
    i2c_stop();
  }
#else
  if( !display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { Serial.println(F("SSD1306 allocation failed")); for(;;);}
#endif
}

/*-------------------------------------------------------*/
void TinyFlip_PrepareDisplayRow( uint8_t y )
{
#if defined(__AVR_ATtiny85__)
  ssd1306_selectPage(y);
#else
  adafruitBuffer = display.getBuffer() + ( y * 128 );
#endif
}

/*-------------------------------------------------------*/
void TinyFlip_SendPixels( uint8_t pixels )
{
#if defined(__AVR_ATtiny85__)
  i2c_write( pixels );
#else
  *adafruitBuffer++ = pixels;
#endif
}

/*-------------------------------------------------------*/
void TinyFlip_FinishDisplayRow()
{
#if defined(__AVR_ATtiny85__)
  i2c_stop();
#endif
}

/*-------------------------------------------------------*/
void TinyFlip_DisplayBuffer()
{
#if !defined(__AVR_ATtiny85__)
  display.display();
#endif
}

/*-------------------------------------------------------*/
void TinyFlip_SerialScreenshot()
{
#if !defined(__AVR_ATtiny85__)
  Serial.println(F("\r\nTinyMinez screenshot"));
  printScreenBufferToSerial( display.getBuffer(), 128, 8 );
#endif
}