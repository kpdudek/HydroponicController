#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include <math.h>

// Pinout list
#define menu_encoder_CLK 3
#define menu_encoder_DT 2
#define menu_encoder_switch 4
#define relay_1 5
#define relay_2 6
#define led_1 7 

// LCD screen variables
LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 16 column and 2 rows
int cursor_r=0,cursor_c=0;
char buffer[20];
int line_index=0;

// Timing variables
unsigned long t=0,t_prev=0,print_prev=0,freq=250;
unsigned long hours=0,minutes=0,seconds=0,milliseconds=0;
unsigned long weeks=0,days=0;

unsigned long int time_on=0,time_off=0,cycles_per_day=0;
unsigned long int t_pump = 0;
bool is_pump_on = false;

// Menu encoder variables
int current_menu_CLK, previous_menu_CLK, menu_encoder_counter=0;
int menu_button_state = 0;

// Menu mode variables
bool is_time_set = false;
bool program_run = false;

void setup() {
  /*
    Sets pinmodes
    Reads first loop of pin states
    Sets up the LCD screen
    Prompts the user to set the time
  */
  pinMode(menu_encoder_CLK,INPUT);
  pinMode(menu_encoder_DT,INPUT);
  pinMode(menu_encoder_switch,INPUT_PULLUP);
  pinMode(relay_1,OUTPUT);
  pinMode(relay_2,OUTPUT);
  pinMode(led_1,OUTPUT);

  // Set initial encoder state
  previous_menu_CLK = digitalRead(menu_encoder_CLK);

  // Set pump interval values
  cycles_per_day = 6;
   time_on = 60000; // minutes to milliseconds
   time_off = 86400000 / cycles_per_day; // 24 hours in milliseconds / cycles per day
//  time_on = 60000; // Test values
//  time_off = 120000; // Test values

  // Initialize the relays to a known state
  digitalWrite(relay_1,HIGH);
  digitalWrite(relay_2,HIGH);
  
  // Setup the LCD screen
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.blink();

  Serial.begin(9600);
}

void loop() {
  // Update loop variables
  t = millis();
  update_menu_controls();
  update_clock();

  // State machine
  if(!is_time_set){
    is_time_set = true;
    program_run = true;
  }
  else if(program_run){
    update_pump();
  }

  // Draw the LCD screen
  draw();

  //Loop execution time
  t_prev = t;
}

void update_clock(){
  milliseconds = milliseconds + (t-t_prev);
  if(milliseconds>=1000){
    seconds++;
    milliseconds=0;
  }
  if(seconds>=60){
    minutes++;
    seconds=0;
  }
  if(minutes>=60){
    hours++;
    minutes=0;
  }
  if(hours>=24){
    days++;
    hours=0;
  }
  if(days>=7){
    weeks++;
    days=0;
  }
}
void update_menu_controls(){
  // Read the encoder
  current_menu_CLK = digitalRead(menu_encoder_CLK);
  if (current_menu_CLK != previous_menu_CLK){ 
    if(digitalRead(menu_encoder_DT) != current_menu_CLK){
      menu_encoder_counter ++;
      } 
    else{
      menu_encoder_counter --;
      }
  }
  previous_menu_CLK = current_menu_CLK;

  cursor_r = menu_encoder_counter;

  // Read the encoder switch state
  menu_button_state = -1*(digitalRead(menu_encoder_switch)-1);
}
void update_pump(){
  if(is_pump_on && t-t_pump >= time_on){
    digitalWrite(relay_1,HIGH);
    digitalWrite(led_1,LOW);
    is_pump_on = false;
    t_pump = t;
  }
  else if(!is_pump_on && t-t_pump >= (time_off-time_on)){
    digitalWrite(relay_1,LOW);
    digitalWrite(led_1,HIGH);
    is_pump_on = true;
    t_pump = t;
  }
}
void draw(){
  if(t-print_prev > freq){
    // lcd.clear();
    lcd.setCursor(0,0);
    sprintf(buffer,"%s","   __Garden Bot__   ");
    lcd.print(buffer);

    lcd.setCursor(0,1);
    // sprintf(buffer,"%-5d",menu_encoder_counter);
    sprintf(buffer,"On (min): %lu",time_on/1000);
    lcd.print(buffer);

    lcd.setCursor(0,2);
    // sprintf(buffer,"%d",menu_button_state);
    sprintf(buffer,"Cycles/day: %lu",cycles_per_day);
//    sprintf(buffer,"Off (min): %lu",time_off/1000);
    lcd.print(buffer);

    lcd.setCursor(0,3);
    sprintf(buffer,"D:%-2lu H:%-2lu M:%-2lu S:%-2lu",days,hours,minutes,seconds);
    lcd.print(buffer);

    lcd.setCursor(cursor_c,cursor_r);

    Serial.println(time_on);
    Serial.println(time_off);
    Serial.println();
    print_prev=t;
  }
}
