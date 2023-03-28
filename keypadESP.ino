#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const long debounceDelay = 20;
const int rows = 4;
const int cols = 4;
const int rowPin[] = {32, 33, 25, 26};
const int colPin[] = {27, 14, 12, 13};
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

class Key {
  public:
  bool State = HIGH;
  bool LastState = HIGH;
  long LastDebounceTime;
  String Value;
};

bool ReadKeyState(Key* key, int pin){
  int read = digitalRead(pin);
  if (read != key->LastState){
    key->LastDebounceTime = millis();
  }
  if((millis() - key->LastDebounceTime) > debounceDelay){
    if(key->State != (read == LOW)){
      key->State = (read == LOW); 
      return true;
    }
  }
  key->LastState = read;
  return false;
}

void displayOled(String str){
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.print(str);
  oled.display();
}

Key keys[4][4];
Key* backspace;
Key* capslock;
Key* lastKey = nullptr;
int pressedCount = 0;
long lastPressedTime = 0;
bool capslocked = false;
String str = "";

void OnKeyPress(Key* key){
  if(key == backspace)
  {
    if(str.length() > 0) str.remove(str.length()-1);
    return;
  }
  if(key == capslock)
  {
    capslocked = !capslocked;
    return;
  }
  if(key == lastKey && (millis()-lastPressedTime) < 1000) 
  {
    int i = ++pressedCount % key->Value.length();
    char value = key->Value[i];
    if(capslocked) value = toupper(value);
    str.setCharAt(str.length()-1, value);
  }
  else {
    str += key->Value[0];
    lastKey = key;
    pressedCount = 0;
  }
  lastPressedTime = millis();
}

void setup() {
  for(int i = 0; i < 4; i++){
    pinMode(colPin[i], OUTPUT);
    pinMode(rowPin[i], INPUT_PULLUP);
  }

  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.setTextColor(WHITE);
  oled.setTextSize(2);
  
  keys[0][0].Value = "1";
  keys[0][1].Value = "2abc";
  keys[0][2].Value = "3def";
  keys[0][3].Value = " ";

  keys[1][0].Value = "4ghi";
  keys[1][1].Value = "5jkl";
  keys[1][2].Value = "6mno";
  backspace = &keys[1][3];
  
  keys[2][0].Value = "7pqrs";
  keys[2][1].Value = "8tuv";
  keys[2][2].Value = "9wxyz";
  capslock = &keys[2][3];

  keys[3][0].Value = "*";
  keys[3][1].Value = "0";
  keys[3][2].Value = "#";
  keys[3][3].Value = "\n";
}

void loop() {
  for(int c = 0; c < cols; c++)
  {
    digitalWrite(colPin[c], LOW);
    for(int r = 0; r < rows; r++)
    {
      Key* key = &keys[r][c];
      bool changed = ReadKeyState(key, rowPin[r]);     
      if(!changed) continue;
      if(!key->State) continue;

      OnKeyPress(key);

      displayOled(str);
    }
    digitalWrite(colPin[c], HIGH);
  }
}
