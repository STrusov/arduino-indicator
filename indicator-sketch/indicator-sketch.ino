// Подключаем библиотеку для работы с жидкокристаллическим
// экраном (англ. Liquid Crystal Display или просто LCD)
#include <LiquidCrystalRus.h>

#include <MsTimer2.h>

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

volatile unsigned ticks = 0;

void tick() {
  ++ticks;
}

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

  MsTimer2::set(1000, tick);
  MsTimer2::start();
}

bool readln(String &str, unsigned timeout = 0) {
  char c;
  if (timeout)
    ticks = 0;
  while (true) {
    while (!Serial.available()) {
      if (timeout && (ticks >= timeout))
        return false;
    }
    c = Serial.read();
    if (c == '\n') {
      MsTimer2::start();
      return true;
    }
    if (c == '\r') continue;
    str += c;
  }
}

// Увеличивает 2х символов, представляющих время.
// Возврашает признак переноса
bool incstrtime(char *p, bool hours = false) {
  if (hours && p[0] == '2' && p[1] == '3') {
    p[0] = p[1] = '0';
    return true;
  }
  if (p[1] < '9') {
    p[1] += 1;
    return false;
  }
  else {
    p[1] = '0';
    if (p[0] < '5') {
      p[0] += 1;
      return false;
    }
    p[0] = '0';
    return true;
  }
}

void loop() {
  String line1("00:00:00");
  while (true) {
    String line, line2;
    if (readln(line, 1) && readln(line2, 1))
      line1 = line;
    else {
      line2 = "                ";
      line1.remove(8, -1);
      line1 += "        ";
      incstrtime(&line1[6]) && incstrtime(&line1[3]) && incstrtime(&line1[0], true);
    }

    // устанавливаем курсор, колонку 0, строку 1. На деле — это
    // левый квадрат 2-й строки, т.к. нумерация начинается с нуля
    lcd.setCursor(0, 0);
    lcd.print(line1);

    lcd.setCursor(0, 1);
    lcd.print(line2);
  }
}
