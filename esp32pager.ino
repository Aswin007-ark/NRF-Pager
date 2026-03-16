
#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// nRF24
RF24 radio(4,5);   // CE, CSN
const byte address[6] = "PAGER";

// Buttons
#define BTN_UP 32
#define BTN_DOWN 33
#define BTN_OK 25

// Buzzer
#define BUZZER 27

// Preset Messages
String presetMessages[] = {
"CALL ME",
"HELLO",
"WHERE ARE YOU",
"MEETING NOW",
"HELP",
"OK",
"COME HERE",
"BUSY",
"GOOD NIGHT"
};

int messageCount = 9;
int currentMessage = 0;

void beep(){
digitalWrite(BUZZER,HIGH);
delay(100);
digitalWrite(BUZZER,LOW);
}

void showMessage(){

display.clearDisplay();
display.setTextSize(1);
display.setTextColor(SSD1306_WHITE);

display.setCursor(0,0);
display.println("SELECT MESSAGE");

display.setCursor(0,25);
display.println(presetMessages[currentMessage]);

display.display();

}

void setup(){

Serial.begin(115200);

pinMode(BTN_UP,INPUT_PULLUP);
pinMode(BTN_DOWN,INPUT_PULLUP);
pinMode(BTN_OK,INPUT_PULLUP);

pinMode(BUZZER,OUTPUT);

Wire.begin(21,22);

display.begin(SSD1306_SWITCHCAPVCC,0x3C);
display.clearDisplay();

display.setCursor(0,10);
display.println("ESP32 PAGER");
display.println("Starting...");
display.display();

delay(2000);

// Start Radio
radio.begin();
radio.openWritingPipe(address);
radio.openReadingPipe(0,address);
radio.setPALevel(RF24_PA_LOW);
radio.startListening();

showMessage();

}

void loop(){

// RECEIVE MESSAGE
if(radio.available()){

char text[32] = "";
radio.read(&text,sizeof(text));

display.clearDisplay();
display.setCursor(0,0);
display.println("NEW MESSAGE");
display.println(text);
display.display();

beep();

delay(3000);

showMessage();
}

// BUTTON UP
if(digitalRead(BTN_UP)==LOW){

currentMessage++;

if(currentMessage>=messageCount)
currentMessage=0;

showMessage();
delay(250);

}

// BUTTON DOWN
if(digitalRead(BTN_DOWN)==LOW){

currentMessage--;

if(currentMessage<0)
currentMessage=messageCount-1;

showMessage();
delay(250);

}

// SEND MESSAGE
if(digitalRead(BTN_OK)==LOW){

radio.stopListening();

char text[32];
presetMessages[currentMessage].toCharArray(text,32);

radio.write(&text,sizeof(text));

radio.startListening();

display.clearDisplay();
display.setCursor(0,20);
display.println("MESSAGE SENT");
display.println(text);
display.display();

beep();

delay(2000);

showMessage();

}

}