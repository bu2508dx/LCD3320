#include <LCD3320.h>

#define di_pin 2   //LCDDI
#define clk_pin 3  //LCDCLK
#define en_pin 4   //LCDEN

LCD3320 lcd(di_pin, clk_pin, en_pin);

void setup() {
            //HZ,  F:,   V,     V:,  °C,   T:,    A,      I:
  lcd.begin(true, true, false, true, true, true, false, true);

}

void loop() {
  int voltage = 100;
  int current = 52;
  int temp = 23;


  for (int i = 0; i < 999; i++) {
    lcd.setFrequency(i);
    lcd.setVoltage(voltage);
    lcd.setCurrent(current);
    lcd.setTemp(temp);
    lcd.update();
    delay(30);
  }
  
  lcd.clear();
  delay(3000);
}
