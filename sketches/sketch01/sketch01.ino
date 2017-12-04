/*
  Ethernet Shield v2 "Hello World"

  This sketch connects to the internet and prints
  the Arduino logo stored in WeMake's wiki

  To run this sketch you need:
  > Arduino UNO
  > Arduino Ethernet Shield v2
  > Tinkerkit LED module connected to OUT6

  edited by WeMake 2017
  based on work by
  David A. Mellis
  Tom Igoe
  Adrian McEwen
 */

#include <SPI.h>
#include <Ethernet2.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x00, 0x00 };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "wiki.wemake.cc";    // name address for WeMake's wiki (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

// connect the LED to pin OUT6
#define ledPin 6

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // set ledPin as OUTPUT
  pinMode(ledPin, OUTPUT);

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /w/success.txt HTTP/1.1");
    client.println("Host: wiki.wemake.cc");
    client.println("Connection: close");
    client.println();

    // Blink the LED when connected to server
    digitalWrite(ledPin, HIGH);
    delay(700);
    digitalWrite(ledPin, LOW);
    delay(700);
    digitalWrite(ledPin, HIGH);
    delay(700);
    digitalWrite(ledPin, LOW);
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}

