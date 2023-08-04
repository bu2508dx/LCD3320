/*===========================================================
|	        	LCD3320 Library			    |
|							    |
|author: Jefferson S. Pereira				    |	
|date: 25/10/2022					    |
|============================================================										
*/

#ifndef LCD3320_h
#define LCD3320_h
#include "Arduino.h"


class LCD3320
{
	public:  //pin LCDDI,   pin LCDCLK,   pin LCDEN
	LCD3320(byte _ddi_pin, byte _clk_pin, byte _en_pin);
	void begin(bool HZ = false, bool FtwoP = false, bool V = false, bool VtwoP = false,
			   bool Celsius = false, bool TtwoP = false, bool A = false, bool ItwoP = false);
	void clear();
	void setFrequency(int num, bool dot2 = false, bool dot1 = false);
	void setVoltage(int num, bool dot2 = false, bool dot1 = false);
	void setTemp(int num, bool dot2 = false, bool dot1 = false);
	void setCurrent(int num, bool dot2 = false, bool dot1 = false);
	void update();


	protected:
	byte _ddi_pin;
	byte _clk_pin;     //           address
	byte _en_pin;      //0x00  0x03  0x06  0x09  0x0a
	byte _sumSymbs[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	void lcdCm(byte lcdmm);
	void lcdStop();
	void setSymbols();
	void clearBuffer();
	void wrByte(byte lcdd);
	void wrBit(bool lcdb);

};
#endif




	
