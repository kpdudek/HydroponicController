#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 16 column and 2 rows

#define menu_encoder_CLK 3
#define menu_encoder_DT 2
#define menu_encoder_switch 4
#define relay_1 5
#define relay_2 6

unsigned long int t,t_prev=0, freq = 1000/5;
int current_menu_CLK, previous_menu_CLK, menu_encoder_counter=0;
int menu_button_state=0;

void setup() {
  pinMode(menu_encoder_CLK,INPUT);
  pinMode(menu_encoder_DT,INPUT);
  pinMode(menu_encoder_switch,INPUT_PULLUP);
  pinMode(relay_1,OUTPUT);
  pinMode(relay_2,OUTPUT);

  previous_menu_CLK = digitalRead(menu_encoder_CLK);
  
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.blink();

  lcd.setCursor(4,0);
  lcd.print("Hello World!");
}

void loop() {
  t = millis();
  // Read the current state of inputCLK
  current_menu_CLK = digitalRead(menu_encoder_CLK);
  if (current_menu_CLK != previous_menu_CLK){ 
    if (digitalRead(menu_encoder_DT) != current_menu_CLK) // counterclockwise
    { 
      menu_encoder_counter ++;
    } 
    else // clockwise
    {
      menu_encoder_counter --;
    }
  }
  previous_menu_CLK = current_menu_CLK;

  // Read the menu encoder switch state
  menu_button_state = -1*(digitalRead(menu_encoder_switch)-1);
  
  if (t-t_prev > freq){
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Hello World!");
    lcd.setCursor(4,1);
    lcd.print(menu_encoder_counter);
    lcd.setCursor(4,2);
    lcd.print(menu_button_state);
     t_prev = t;
  }
}
