//lcd
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//RFID
#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>

#define sda 10
#define rst 9

MFRC522 mfrc(sda, rst);

//소프트웨어시리얼
#include <SoftwareSerial.h>

SoftwareSerial soft_serial(4, 5); //RX4 TX5

//상품
int product[5];
int productSel = 0;

char*tempSav[5];
String clientInfo[5];
String productInfo[5];

int parsing(char parsChar[]);



//외부인터럽트
void confirm()
{
  Serial.println("interrupt send");
  switch (productSel) {
      case 1:
        soft_serial.print("c1E");
        Serial.print("c1E");
        break;
      case 2:
        soft_serial.print("c2E");
        Serial.print("c2E");
        break;
      default:
        soft_serial.print("c0E");
        Serial.print("c0E");
        break;
  }
}

void setup() {
  Serial.begin(9600);

  soft_serial.begin(9600);

  SPI.begin();
  mfrc.PCD_Init();

  lcd.init();
  lcd.backlight();

  pinMode(7, INPUT);
  pinMode(8, INPUT);
  attachInterrupt(1, confirm, RISING);
}

const int dur = 1000;
unsigned long pre = 0;
unsigned long cur = 0;

void loop() {
  cur = millis();
  if (cur - pre > dur) {
    product[0] = digitalRead(7);
    product[1] = digitalRead(8);
    if (product[0] == 1) {
      productSel = 1;
      Serial.println("(product[0] == 1)");
    }
    else if (product[1] == 1) {
      productSel = 2;
      Serial.println("(product[1] == 1)");
    }
    else {
      Serial.println(productSel);
    }

    //RFID
    int rfid_flag = 0;
    if (!mfrc.PICC_IsNewCardPresent()) {
      rfid_flag++;
    }
    if (!mfrc.PICC_ReadCardSerial()) {
      rfid_flag++;
    }
    if (rfid_flag == 0)
    {
      soft_serial.print('a');//데이터 구분
      Serial.print('a');
      for (byte i = 0; i < 4; i++) {
        soft_serial.print(mfrc.uid.uidByte[i]);
        Serial.print(mfrc.uid.uidByte[i]);
        if (i == 3) break;
        soft_serial.print(" ");
        Serial.print(" ");
      }
      soft_serial.print("E");
      Serial.println("E");
    }
    pre = cur;
  }

  //데이터받기
  char reciveChar[20] = {'\0'};
  int reciveInt = 0;
  while (soft_serial.available()) {
    Serial.println("READ");
    reciveChar[reciveInt] = soft_serial.read();
    reciveInt++;
    delay(1);//수신딜레이
  }
  
  int parsLength = 0;
  if (reciveChar[0] == '1') {
    Serial.println(reciveChar);
    lcd.clear();
    lcd.print("No Information");
  }
  else if (reciveChar[0] == 'b') {
    parsLength = parsing(reciveChar);
    for (int i = 0; i < parsLength; i++) {
      clientInfo[i] = tempSav[i];
      Serial.println(clientInfo[i]);
    }
    lcd.setCursor(0, 0);
    lcd.print("Name:     ");
    lcd.setCursor(5, 0);
    lcd.print(clientInfo[1]);
    lcd.setCursor(10, 0);
    lcd.print("Age:  ");
    lcd.setCursor(14, 0);
    lcd.print(clientInfo[2]);
    lcd.setCursor(0, 1);
    lcd.print("Money:          ");
    lcd.setCursor(6, 1);
    lcd.print(clientInfo[3]);
  }
  else if (reciveChar[0] == 'd') {
    parsLength = parsing(reciveChar);
    for (int i = 0; i < parsLength; i++) {
      productInfo[i] = tempSav[i];
      Serial.println(productInfo[i]);
    }
    lcd.setCursor(0, 0);
    lcd.print("ID:       ");
    lcd.setCursor(3, 0);
    lcd.print(productInfo[1]);

    lcd.setCursor(10, 0);
    lcd.print("Pos:  ");
    lcd.setCursor(14, 0);
    if (clientInfo[2].toInt() < productInfo[2].toInt()) {
      lcd.print("NO");
    }
    else {
      lcd.print("OK");
    }
    lcd.setCursor(0, 1);
    lcd.print("Price:          ");
    lcd.setCursor(6, 1);
    if (clientInfo[3].toInt() < productInfo[3].toInt()) {
      lcd.print("NO MONEY");
    }
    else {
      lcd.print(productInfo[3]);

    }
  }
}

int parsing(char parsChar[])
{
  char*ptr = strtok(parsChar, " ");
  int parsLength = 0;
  while (ptr != NULL)
  {
    tempSav[parsLength] = ptr;
    ptr = strtok(NULL, " ");
    parsLength++;
  }
  return parsLength;
}
