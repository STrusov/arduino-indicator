// Подключаем библиотеку для работы с жидкокристаллическим
// экраном (англ. Liquid Crystal Display или просто LCD)
#include <LiquidCrystalRus.h>

// Объявляем объект, для управления дисплеем. Для его создания
// необходимо указать номера пинов, к которым он подключен в
// порядке:       RS   E DB5 DB6 DB7 DB8
LiquidCrystalRus lcd(13, 12, 11, 10,  9,  8);

byte degreeCelsius[8] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
};

void setup() {
  // начинаем работу с экраном. Сообщаем объекту количество
  // строк и столбцов. Опять же, вызывать pinMode не требуется:
  // функция begin сделает всё за нас
  lcd.begin(16, 2);
  // печатаем сообщение на первой строке
  lcd.print("****************");
  lcd.createChar('\1', degreeCelsius);

//  Serial.begin(115200);
  Serial.begin(9600);
  Serial.println("Ok");
}

String readln() {
  String str;
  char c;
  while (true) {
    while (!Serial.available()) {}
    c = Serial.read();
    if (c == '\n') return str;
    if (c == '\r') continue;
    str += c;
  }
}

void lcd_println16(const String& str) {
  lcd.print(str);
  for (size_t i = 16 - str.length(); i > 0; --i) {
    lcd.print(' ');
  }
}

void loop() 
{
  // устанавливаем курсор, колонку 0, строку 1. На деле — это
  // левый квадрат 2-й строки, т.к. нумерация начинается с нуля
  lcd.setCursor(0, 0);
  lcd.print(readln());

  lcd.setCursor(0, 1);
  lcd.print(readln());

}
