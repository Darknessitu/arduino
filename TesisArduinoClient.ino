#include <SoftwareSerial.h>
#include <TimeLib.h>

int sueloValue = 0; // variable to store the value coming from the soil sensor
int lluviaValue = 0; // variable to store the value coming from the rain sensor
int fakeDay = 1;

String ssid = "DTVNET_AC2975";
String password = "oxdxwucb";
String server = "52.17.165.162";
String uri = "/add/value/sensor";

SoftwareSerial SerialESP8266(10, 11); // RX, TX

void reset() {
  SerialESP8266.println("AT+RST");
  delay(1000);
  if (SerialESP8266.find("OK") ) Serial.println("Module Reset");
}

void setup() {
  Serial.begin(9600);
  SerialESP8266.begin(9600);

  SerialESP8266.println("AT"); //Verificamos si el ESP8266 responde
  delay(1000);
//  if (SerialESP8266.find("OK"))
//    Serial.println("Respuesta AT correcto");
//  else
//    Serial.println("Error en ESP8266");

  //-----Configuración de red-------//

  //ESP8266 en modo estación (nos conectaremos a una red existente)
//  SerialESP8266.println("AT+CWMODE=1");
//  if (SerialESP8266.find("OK"))
//    Serial.println("ESP8266 en modo Estacion");

  //Nos conectamos a una red wifi
  String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
  SerialESP8266.println(cmd);
  //Serial.println("Conectandose a la red ...");
  SerialESP8266.setTimeout(20000); //Aumentar si demora la conexion
  if (SerialESP8266.find("OK"))
    Serial.println("WIFI conectado");
  //else
  //  Serial.println("Error al conectarse en la red");
  SerialESP8266.setTimeout(2000);
  //------fin de configuracion-------------------
  delay(1000);

}

void loop() {
  // read the value from the sensor:
  sueloValue = analogRead(A0);
  lluviaValue = analogRead(A1);
  Serial.print("Suelo = " );
  Serial.println(sueloValue);

  Serial.print("Lluvia = " );
  Serial.println(lluviaValue);
  sendValue("56154123",String(sueloValue), "2361341", String(lluviaValue));
  delay(10000);
}

void sendValue(String sueloId, String sueloVal, String lluviaId, String lluviaVal) {
  SerialESP8266.println("AT+CIPSTART=\"TCP\",\"" + server + "\",8080");//start a TCP connection.
  if ( SerialESP8266.find("OK")) {
    //Serial.println("TCP connection ready");
  }

  delay(1000);

  String now = "2017-05-" + getDigits(fakeDay) + " 22:00:00" ;
  //String data = "{\"serial\":\"" + sensorId + "\",\"date\":\"" + now + "\",\"value\":" + val + "}";
  String data = "{\"date\":\"" + now + "\",\"items\":[{\"serial\":\""+ sueloId + "\",\"value\":" + sueloVal + "},{\"serial\":\"" + lluviaId + "\",\"value\":" + lluviaVal + "}]}";
  //Serial.println(data);
  String postRequest = "POST " + uri + " HTTP/1.0\r\n" +

                       "Host: " + server + "\r\n" +

                       "Accept: *" + "/" + "*\r\n" +

                       "Content-Length: " + data.length() + "\r\n" +

                       "Content-Type: application/json\r\n" +

                       "\r\n" + data;

  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.

  SerialESP8266.print(sendCmd);

  SerialESP8266.println(postRequest.length() );

  delay(500);

  if (SerialESP8266.find(">")) {
    //Serial.println("Sending..");
    SerialESP8266.print(postRequest);

    if ( SerialESP8266.find("SEND OK")) {
      Serial.println("Packet sent");

      while (SerialESP8266.available()) {
        String tmpResp = SerialESP8266.readString();
 //       Serial.println(tmpResp);
      }
      // close the connection
      SerialESP8266.println("AT+CIPCLOSE");
      fakeDay++;
    }
  }
}

String getDigits(int digits) {
  String digit = "";
  if (digits < 10)
    digit += "0";
  digit += String(digits);
  return digit;
}
