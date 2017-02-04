void handle_wifi_scan() {
 int n = WiFi.scanNetworks();
 DynamicJsonBuffer jsonBuffer;
 JsonObject& json = jsonBuffer.createObject();
 JsonArray& networks = json.createNestedArray("networks");
 for(int i=0;i<n;i++) {
  JsonObject& data = networks.createNestedObject();
  data["ssid"] = WiFi.SSID(i);
  data["pass"] = (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "" : "*";
  data["dbm"] = WiFi.RSSI(i);
  //data["bssid"] = WiFi.BSSIDstr(i);
  //data["channel"] = WiFi.channel(i);
  //data["isHidden"] = WiFi.isHidden(i);
 }
 String root;
 json.printTo(root);
 HTTP.send(200, "text/json", root);
}

void webUpdateSpiffs() {
 String refresh = "<html><head><meta http-equiv=\"refresh\" content=\"1;http://";
 refresh += WiFi.localIP().toString();
 refresh += "\"></head></html>";
 HTTP.send(200, "text/html", refresh);
 // t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs("http://backup.privet.lv/rgb_spiffs_1m_256k.bin");
}

// Перезагрузка модуля
void handle_restart() {
 String restart = HTTP.arg("device");
 if (restart == "ok") {                         // Если значение равно Ок
  HTTP.send(200, "text/plain", "Reset OK"); // Oтправляем ответ Reset OK
  ESP.restart();                                // перезагружаем модуль
 } else {                                        // иначе
  HTTP.send(200, "text/plain", "No Reset"); // Oтправляем ответ No Reset
 }
}

// Меняет флаг для запуска сервопривода
void MotorActiv() {
 chaing = 1;
 HTTP.send(200, "text/plain", "OK");
}

// Сохраняет все настройки в файле
void handle_save_config() {
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

// Время вращения сервопривода
void handle_time_servo1() {
 TimeServo1 = HTTP.arg("t").toInt();
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

// Время вращения сервопривода
void handle_time_servo2() {
 TimeServo2 = HTTP.arg("t").toInt();
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

// Установка времянной зоны
void handle_time_zone() {
 timezone = HTTP.arg("timezone").toInt();
 Time_init(timezone);
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}
// Установка языка
void handle_leng() {
 Language = HTTP.arg("set");
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

// Установка параметров сети
void handle_ssid() {
 _ssid = HTTP.arg("ssid");
 _password = HTTP.arg("password");
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

// Установка параметров сети
void handle_ddns() {
 DDNS = HTTP.arg("url");
 DDNSName = HTTP.arg("wanurl");
 DDNSPort = HTTP.arg("wanport").toInt();
 //Serial.println(HTTP.arg("url"));
 //Serial.println(HTTP.arg("wanurl"));
 ip_wan();
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

// Установка параметров сети
void handle_ssdp() {
 SSDP_Name = HTTP.arg("ssdp");
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

//Установка параметров точки доступа
void handle_ssidap() {
 _ssidAP = HTTP.arg("ssidAP");
 _passwordAP = HTTP.arg("passwordAP");
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

//Время из сети
void handle_time() {
 Time_init(timezone);
 String Time = XmlTime();
 HTTP.send(200, "text/plain", "OK: " + Time);
}

//Установка количество оборотов
void handle_turn() {
 turn = HTTP.arg("turn").toInt();
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

//Установка скорости вращения
void handle_speed() {
 speed = HTTP.arg("speed").toInt();
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

//Задать время открытия
void handle_time_up() {
 TimeUp = HTTP.arg("open");
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

//Задать время закрытия
void handle_time_down() {
 TimeDown = HTTP.arg("close");
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

//Задать время закрытия
void handle_calibration() {
 calibration = HTTP.arg("calibration").toInt();
 saveConfig();
 HTTP.send(200, "text/plain", "OK");
}

void HTTP_init(void) {
 // SSDP дескриптор
 HTTP.on("/description.xml", HTTP_GET, []() {
  SSDP.schema(HTTP.client());
 });
 // Добавляем функцию Update для перезаписи прошивки по WiFi при 1М(256K SPIFFS) и выше
 httpUpdater.setup(&HTTP);
 HTTP.on("/webupdatespiffs", webUpdateSpiffs);                // Обнавление FS из интернет
 HTTP.on("/restartWiFi", RestartWiFi);                // Переплдключение WiFi при первом старте
 HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
 HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
 HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
 //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год
 HTTP.on("/motor", MotorActiv);            // запуск мотора напровление храниться в переменной
 HTTPWAN.on("/motor", MotorActiv);            // запуск мотора напровление храниться в переменной
 HTTP.on("/Timeservo1", handle_time_servo1);  // установка времени вращения сервопривода
 HTTP.on("/Timeservo2", handle_time_servo2);// установка времени вращения сервопривода
 HTTP.on("/wifi.scan.json", handle_wifi_scan);      // сканирование ssid
 HTTP.on("/TimeZone", handle_time_zone);    // Установка времянной зоны
 HTTP.on("/Time", handle_time);            // Синхронизировать время из сети
 HTTP.on("/TimeUp", handle_time_up);       // Установить время открытия
 HTTP.on("/TimeDown", handle_time_down);   // Установить время закрытия
 HTTP.on("/ssdp", handle_ssdp);        // Установить имя устройства
 HTTP.on("/ssid", handle_ssid);        // Установить имя и пароль роутера
 HTTP.on("/ssidap", handle_ssidap);    // Установить имя и пароль для точки доступа
 HTTP.on("/speed", handle_speed);          // Установить скорость вращения сервопривода
 HTTP.on("/turn", handle_turn);          // Установить оборотов
 HTTP.on("/Save", handle_save_config);      // Сохранить настройки в файл
 HTTP.on("/configs.json", handle_config);  // формирование config_xml страницы для передачи данных в web интерфейс
 HTTP.on("/devices.scan.json", inquirySSDP);  // формирование iplocation_xml страницы для передачи данных в web интерфейс
 HTTP.on("/devices.list.json", handle_ip_list);  // формирование iplocation_xml страницы для передачи данных в web интерфейс
 HTTP.on("/calibration", handle_calibration);         // колибруем серву
 HTTP.on("/restart", handle_restart);                 // Перезагрузка модуля
 HTTP.on("/lang", handle_leng);               // Установить язык
 HTTP.on("/ddns", handle_ddns);               // Установить DDNS
 HTTP.on("/lang.list.json", handle_leng_list);               // Установить язык
 HTTP.on("/modules.json", handle_modules);               // Узнать какие модули есть в устройстве
 // Запускаем HTTP сервер
 HTTP.begin();
 HTTPWAN.begin();
}

// Получение текущего времени
String XmlTime(void) {
 String Time = ""; // Строка для результатов времени
 int i = 0; // Здесь будем хранить позицию первого символа :
 time_t now = time(nullptr); // получаем времяс помощью библиотеки time.h
 Time += ctime(&now); // Преобразуем время в строку формата
 i = Time.indexOf(":"); //Ишем позицию первого символа :
 Time = Time.substring(i - 2, i + 6); // Выделяем из строки 2 символа перед символом : и 6 символов после
 return Time; // Возврашаем полученное время
}

void handle_config() {
 String root = "{}";  // Формировать строку для отправки в браузер json формат
 DynamicJsonBuffer jsonBuffer;
 //  вызовите парсер JSON через экземпляр jsonBuffer
 JsonObject& json = jsonBuffer.parseObject(root);
 // Заполняем поля json
 json["DDNS"] = DDNS;  // Имя DDNS
 json["DDNSName"] = DDNSName;  // Имя DDNSName
 json["DDNSPort"] = DDNSPort;  // Имя DDNSPort
 json["SSDP"] = SSDP_Name; // Имя SSDP
 json["ssid"] = _ssid; // Имя сети
 json["password"] = _password; // Пароль сети
 json["ssidAP"] = _ssidAP; // Имя точки доступа
 json["passwordAP"] = _passwordAP; // Пароль точки доступа
 json["timezone"] = timezone; // Времянная зона
 json["speed"] = speed; // Скорость вращения
 json["turn"] = turn; // Обороты
 json["timeservo1"] = TimeServo1; //  Время врашения
 json["timeservo2"] = TimeServo2; //  Время врашения
 json["TimeUp"] = TimeUp; // Время открытия
 json["TimeDown"] = TimeDown; // Время закрытия
 json["time"] = XmlTime(); // Текущее время
 json["calibration"] = calibration; // Колибруе серву
 json["state"] = state0; // Статус
 if (Language == NULL) { // Язык
  json["lang"] = "ru";
 } else {
  json["lang"] = Language;
 }
 root="";
 json.printTo(root);
 HTTP.send(200, "text/json", root);
}

void handle_ip_list() {
 HTTP.send(200, "text/json", "[{\"ip\":\""+WiFi.localIP().toString()+"\"}"+Devices+"]");
}

void handle_ip_scan() {
 inquirySSDP();
 String json = "";
 //Serial.println(Devices);
 if (Devices != "") {
  json = Devices;
  //json += ",";
 }
 json +=modules();
 DevicesList = "["+json+"]";
 //Serial.println(json);
 HTTP.send(200, "text/json", "[" + json + "]");
 Devices="";
}

void handle_modules() {
 DynamicJsonBuffer jsonBuffer;
 JsonObject& json = jsonBuffer.createObject();
 json["SSDP"] = SSDP_Name;
 json["state"] = state0;
 JsonArray& data = json.createNestedArray("module");
 for (int i = 0; i < sizeof(module) / sizeof(module[0]); i++) {
  data.add(module[i]);
 }
 String root;
 json.printTo(root);
 HTTP.send(200, "text/json", root);
}

String modules() {
 String json = "";
 for (int i = 0; i <= sizeof(module) / sizeof(module[0]); i++) {
  json += "{\"ip\":\"";
  json += WiFi.localIP().toString();
  json += "\",\"module\":\"";
  json += module[i];
  //Serial.println(module[i]);
  json += "\"";
  json += "}";
  if (i != sizeof(module) / sizeof(module[0])) json += ",";
 }
 return json;
}

void handle_leng_list() {
 HTTP.send(200, "text/json", Lang);
}

