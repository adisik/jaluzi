// ------------------- Инициализация  Динамика
void initBuzer() {
  int pin = readArgsInt();
  sendOptions("pinBuzer", pin);
  pinMode(pin, OUTPUT);
  sendStatus("stateBuzer", readArgsInt());
  analogWrite(pin,   getStatusInt("stateBuzer"));
  analogWriteFreq(0);
}

void buzerBeep() {
  analogWrite(jsonReadtoInt(configLive, "pinBuzer"), readArgsInt());
  analogWriteFreq(readArgsInt());
}


// -----------------  Аналоговый вход A0
void initA0() {
  HTTP.on("/analog.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", graf(analogRead(A0), 10, 3000, "low:0"));
  });
  modulesReg("analog");
}
// ----------------- Кнопка GPIO 0
void initTach() {
  // Кнопка будет работать по прерыванию
  uint8_t TACH_PIN = 0;
  attachInterrupt(TACH_PIN, Tach_0, RISING); //прерывание сработает, когда состояние вывода изменится с низкого уровня на высокий
  configOptions = jsonWrite(configOptions, "TachCommand", readArgsString());
}
// Выполняется при нажатии на кнопку
void Tach_0() {
  static unsigned long millis_prev;
  // Устроняем дребезг контакта
  if (millis() - 500 > millis_prev) {
    command = jsonRead(configOptions, "TachCommand");
  }
  millis_prev = millis();
}

// -----------------  Движение
void initMotion() {
  int pin = readArgsInt();
  pinMode(pin, INPUT);
  attachInterrupt(pin, motionOn, RISING); //прерывание сработает, когда состояние вывода изменится с низкого уровня на высокий
  String cmd = readArgsString();
  configOptions = jsonWrite(configOptions, "Command", cmd);
  modulesReg("movement");
}

void motionOn() {
  motion.attach(120, motionOff);
  command = jsonRead(configOptions, "Command") + "on";

}
void motionOff() {
  motion.detach();
  command = jsonRead(configOptions, "Command") + "off";

}

// -----------------  DHT
void initDHT() {
  dht.setup(readArgsInt());
  //delay(dht.getMinimumSamplingPeriod());
  delay (1000);

  String temp = "";
  temp += dht.getTemperature();
  if (temp != "nan") {
    HTTP.on("/temperature.json", HTTP_GET, []() {
      float temp = dht.getTemperature();
      if (temp == 'NaN') {
        temp = 0;
      }
      HTTP.send(200, "application/json", graf(temp, 10, 3000, "low:0"));
    });
    modulesReg("temperature");
    HTTP.on("/humidity.json", HTTP_GET, []() {
      float temp = dht.getHumidity();;
      if (temp == 'NaN') {
        temp = 0;
      }
      HTTP.send(200, "application/json", graf(temp, 10, 3000, "low:0"));
    });
    modulesReg("humidity");
  }
}

// ---------- initD18B20
void initD18B20() {
  pinMode(d18b20PIN, INPUT_PULLUP);
  d18b20.begin();
  d18b20.requestTemperatures();
  float ok = d18b20.getTempCByIndex(0);
  d18b20.setResolution(12);
  if (ok != -127) {
    HTTP.on("/temperature.json", HTTP_GET, []() {
      d18b20.requestTemperatures();
      float temp = d18b20.getTempCByIndex(0);
      if (temp == -127) {
        temp = 0;
      }
      HTTP.send(200, "application/json", graf(temp, 10, 3000));
    });
    modulesReg("temperature");
  }
}
// ----------------------Приемник на 433мГ
void rfReceived() {
  byte pin = readArgsInt();
  if (pin == 1 || pin == 3)  Serial.end();
  mySwitch.enableReceive(pin);
  // задача опрашивать RC код
  ts.add(3, 5, [&](void*) {
    RCRCreceiv();
  }, nullptr, true);
  sendStatus("Received", 0);
  modulesReg("rfReceived");
}

void RCRCreceiv() {
  if (mySwitch.available()) {
    int value = mySwitch.getReceivedValue();
    if (value == 0) {
      configJson = jsonWrite(configJson, "Received", 0);
    } else {
      int codeRC = mySwitch.getReceivedValue();
      flag = sendStatus("Received", codeRC);
    }
    mySwitch.resetAvailable();
  }
}


