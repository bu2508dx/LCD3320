/*===========================================================
|						LCD3320 Library						|
|															|
|author: Jefferson S. Pereira								|	
|date: 25/10/2022											|
|============================================================										
*/


#include "LCD3320.h"
						   // 0     1...                                         ...9     F											
const byte disptab1[11] = { 0xaf, 0xa0, 0xcb, 0xe9, 0xe4, 0x6d, 0x6f, 0xa8, 0xef, 0xed, 0x4e};  //Segments table 0 to 9 and "F" for frequency and voltage
const byte disptab2[20] = { 0X05, 0x05, 0x03, 0x07, 0x07, 0x06, 0x06, 0x05, 0x07, 0x07,
							0xf0, 0x00, 0xd0, 0x90, 0x20, 0xb0, 0xf0, 0x10, 0xf0, 0xb0};  		//Segments table 0 to 9  for temperatura

const byte disptab3[11] = { 0x5f, 0x50, 0x3d, 0x79, 0x72, 0x6b, 0x6f, 0x51, 0x7f, 0x7b, 0x27};  //Segments table 0 to 9 and "F" for current

byte buffer[13] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  //buffer

				//pin LCDDDI,   pin LCDCLK,   pin LCDEN
LCD3320::LCD3320(byte _ddi_pin, byte _clk_pin, byte _en_pin) {
	this->_ddi_pin = _ddi_pin;
	this->_clk_pin = _clk_pin;
	this->_en_pin = _en_pin;
}
						//Symbols of display HZ, F:, V, V:, °C, T:, A, I:          
void LCD3320::begin(bool HZ = false, bool FtwoP = false, bool V = false, bool VtwoP = false,
	bool Celsius = false, bool TtwoP = false, bool A = false, bool ItwoP = false) {
	
	pinMode(_ddi_pin, OUTPUT);
	pinMode(_clk_pin, OUTPUT);
	pinMode(_en_pin, OUTPUT);
	digitalWrite(_ddi_pin, HIGH);
	digitalWrite(_clk_pin, HIGH);
	digitalWrite(_en_pin, HIGH);
	lcdCm(0x18);
	lcdCm(0x01); 
	lcdCm(0x03);
	lcdCm(0x29);
	lcdCm(0x04);
	clear();        
	if(HZ) 	    _sumSymbs[0] =  0x10; 
	if(FtwoP)   _sumSymbs[3] =  0x01; 
	if(V)       _sumSymbs[1] =  0x10; 
	if(VtwoP)   _sumSymbs[2] =  0x80; 
	if(Celsius) _sumSymbs[2] += 0x08; 
	if(TtwoP)   _sumSymbs[3] += 0x04; 
	if(A)       _sumSymbs[4] =  0x80;  
	if(ItwoP)   _sumSymbs[2] += 0x20; 
}

void LCD3320::lcdCm(byte lcdmm) {  //Clear LCD memory for as described in the datasheet
	digitalWrite(_en_pin, LOW);
	wrBit(1);
	wrBit(0);
	wrBit(0);
	wrByte(lcdmm);
	wrBit(0);
	lcdStop();
}

void LCD3320::lcdStop() {
	digitalWrite(_en_pin, HIGH);
	digitalWrite(_ddi_pin, HIGH);
}

void LCD3320::clear() {
	clearBuffer();     
	digitalWrite(_en_pin, LOW);
	wrBit(1);
	wrBit(0);  //write command
	wrBit(1);
	for (int i = 0; i < 6; i++)  //auto adds increment	mode
		wrBit(0);
	for (int i = 0; i < 13; i++)  //all segments off
		wrByte(0x00); 
	lcdStop();
}

void LCD3320::setSymbols(){
	buffer[0] += _sumSymbs[0];
	buffer[3] += _sumSymbs[1];
	buffer[6] += _sumSymbs[2];
	buffer[9] += _sumSymbs[3];
	buffer[10] += _sumSymbs[4];
}

void LCD3320::clearBuffer() {
	for (int i = 0; i < 13; i++)  
    buffer[i] = 0x00;
}

void LCD3320::setFrequency(int num, bool dot2 = false, bool dot1 = false) {
	if(num >999){
		buffer[0] = disptab1[10];
		buffer[1] = disptab1[10];   //overflow FFF
		buffer[2] = disptab1[10];
		return;
	}
	
	if (num > 99)               //with dot	       //without dot
		buffer[2] = dot2 ? disptab1[num / 100] + 0x10 : disptab1[num / 100];  //hundreds
	else
		buffer[2] = dot2 ? disptab1[0] + 0x10 : disptab1[0];
	
	
	if (num > 9) {
		num %= 100;              //with dot	       //without dot
		buffer[1] = dot1 ? disptab1[num / 10] + 0x10 : disptab1[num / 10];  //dozens
	} else
		buffer[1] = dot1 ? disptab1[0] + 0x10 : disptab1[0];
	
	
	buffer[0] = disptab1[num % 10];  //unitys
}

void LCD3320::setVoltage(int num, bool dot2 = false, bool dot1 = false) {
	if(num >999){
		buffer[3] = disptab1[10];
		buffer[4] = disptab1[10];   //overflow FFF
		buffer[5] = disptab1[10];
		return;
	}
		
	if (num > 99)                 //with dot     	        //without dot        
		buffer[5] = dot2 ? disptab1[num / 100]  + 0x10  : disptab1[num / 100];  //hundreds
	else
		buffer[5] = dot2 ? disptab1[0] + 0x10 : disptab1[0];
	
	
	if (num > 9) {
		num %= 100;              //with dot              //without dot        
		buffer[4] = dot1 ? disptab1[num / 10] + 0x10 : disptab1[num / 10] ;  //dozens
	} else
		buffer[4] = dot1 ? disptab1[0] + 0x10 : disptab1[0] ;
	
	
	buffer[3] = disptab1[num % 10];  //unitys
}

void LCD3320::setTemp(int num, bool dot2 = false, bool dot1 = false) {
	if(num >999){
		buffer[6] = 0x02; 
		buffer[7] = 0x72;  //overflow FFF
		buffer[8] = 0x72; 
		buffer[9] = 0X70; 
		return;
	}
	
	
	buffer[6] = disptab2[num % 10];      //unityss
	buffer[7] = disptab2[num % 10 + 10];
	
	if (num > 9) {			       //with dot                        //without dot
		buffer[7] += dot1 ? disptab2[num % 100 / 10] + 0x08 : disptab2[num % 100 / 10];  //dozens
		buffer[8] = disptab2[num % 100 / 10 + 10];
		} else {						
			buffer[7] += dot1 ? disptab2[0] + 0x08 : disptab2[0] ;
			buffer[8] = disptab2[10]; 
	}
	
	
	if (num > 99) {                //withdot               //without dot 	         
		buffer[8] += dot2 ? disptab2[num / 100] + 0x08 : disptab2[num / 100];  //hundreds
		buffer[9] = disptab2[num / 100 + 10];
		} else {
			buffer[8] += dot2 ? disptab2[0] + 0x08 : disptab2[0];
			buffer[9] = disptab2[10];
	}
}

void LCD3320::setCurrent(int num, bool dot2 = false, bool dot1 = false) {
	if(num >999){	
		buffer[10] = disptab3[10];
		buffer[11] = disptab3[10];
		buffer[12] = disptab3[10];
		return;
	}
	
	if (num > 99)                //with dot                     //without dot	        
		buffer[12] = dot2 ? disptab3[num / 100] + 0x80 : disptab3[num / 100];  //hundreds
	else
		buffer[12] = dot2 ? disptab3[0] + 0x80 : disptab3[0];
	
	
	if (num > 9) {
		num %= 100;              //with dot               //without dot
		buffer[11] = dot1 ? disptab3[num / 10] + 0x80 : disptab3[num / 10];  //dozens
	} else
		buffer[11] = dot1 ? disptab3[0] + 0x80 : disptab3[0];
	
	
	buffer[10] = disptab3[num % 10];  //unityss
}

void LCD3320::update() {
	setSymbols();	
	digitalWrite(_en_pin, LOW);
	wrBit(1);
	wrBit(0);  //write command
	wrBit(1);
	for (int i = 0; i < 6; i++) wrBit(0);
	for (int i = 0; i < 13; i++) wrByte(buffer[i]);
	lcdStop();
	clearBuffer();
}

void LCD3320::wrByte(byte lcdd) {
	for (int i = 7; i >= 0; i--)
		wrBit(lcdd & (1 << i) ? HIGH : LOW);
}

/************ low level **********/
void LCD3320::wrBit(bool lcdb) {
	digitalWrite(_ddi_pin, lcdb);
	digitalWrite(_clk_pin, LOW);
	digitalWrite(_clk_pin, HIGH);
}