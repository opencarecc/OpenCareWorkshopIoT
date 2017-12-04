/*
  Arduino Ethernet Shield v2 & IFTTT - Analog values

 This sketch connects to IFTTT and execute an Applet
 through the Webhooks service sending values from an
 analog sensor
 
 To run this sketch you need:
 > an Arduino UNO
 > an Arduino Ethernet shield ver2
 > a Tinkerkit LED module connected to OUT6
 > a Tinkerkit Button module connected to IN3
 > a Tinkerkit Potentiometer module connected to IN2
 > an IFTTT Applet that receives a web request through
 the Webhooks service and then does something :)
 
 After uploading the sketch open the Serial Monitor!


 created Nov 2017 by WeMake
 based on work by
 Erico Porto
 Neil Webber
 
 */

#include <SPI.h>
#include <Ethernet2.h>
#include <EEPROM.h>

// Ethernet MAC address to use.
//     Usually Arduino Ethernet shields come with an address printed on
//     them (sometimes on underside of the board). Use that one here.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x00, 0x00 };  // REPLACE WITH REAL ADDRESS

// IFTTT parameters:
//   Key -- Obtained when setting up/connecting the Webooks service in IFTTT //see--->https://ifttt.com/maker_webhooks
char IFTTT_Key[] = "insert_key_here";
//   Event -- Arbitrary name for the event; used in the IFTTT Applet.
char IFTTT_Event[] = "button_pressed";

// helper functions for constructing the POST data
// append a string or int to a buffer, return the resulting end of string
char *append_str(char *here, char *s) {
    while (*here++ = *s++);
    return here-1;
}
char *append_ul(char *here, unsigned long u) {
    char buf[20];       // we "just know" this is big enough
    return append_str(here, ultoa(u, buf, 10));
}

// connect the LED to pin OUT6
#define ledPin 6

// connect the button to pin IN3
#define buttonPin A3

// connect the potentiometer to pin IN2
#define potPin A2
int val;  // variable to store the value of the potentiometer


void setup() {
  // this sets up the network connection, including IP addr via DHCP
  Ethernet.begin(mac);

  pinMode(ledPin, OUTPUT);  // set ledPin as output
  digitalWrite(ledPin, LOW);  // turn off ledPin

  Serial.begin(9600);
}

void loop() {
  // DHCP lease check/renewal (library only sends request if expired)
  Ethernet.maintain();
  
  //while (!Serial); // wait for a serial connection

  // read values from the potentiometer
  val = analogRead(potPin);
  Serial.print("Waiting for BUTTON... - Pot value: ");
  Serial.println(val);
  
  // check the "switch state" of the sensor
  // if it is pushed, connect and turn on the LED
  if (digitalRead(buttonPin) == HIGH) {     
    Serial.println("Trying to connect...");
    send_event();
    digitalWrite(ledPin, HIGH);     // turn LED on
    delay(1000);         
  } else {
    digitalWrite(ledPin, LOW);    // turn LED off
  }
}


void send_event() {
    // Initialize the client library
    EthernetClient client = EthernetClient();

    if (client.connect("maker.ifttt.com", 80))
      {
        Serial.println("Connected to IFTTT");  
        // construct the POST request
        char post_rqst[256];    // hand-calculated to be big enough
      
        char *p = post_rqst;
        p = append_str(p, "POST /trigger/");
        p = append_str(p, IFTTT_Event);
        p = append_str(p, "/with/key/");
        p = append_str(p, IFTTT_Key);
        p = append_str(p, " HTTP/1.1\r\n");
        p = append_str(p, "Host: maker.ifttt.com\r\n");
        p = append_str(p, "Content-Type: application/json\r\n");
        p = append_str(p, "Content-Length: ");

      
        // we need to remember where the content length will go, which is:
        char *content_length_here = p;
      
        // it's always two digits, so reserve space for them (the NN)
        p = append_str(p, "NN\r\n");
      
        // end of headers
        p = append_str(p, "\r\n");
      
        // construct the JSON; remember where we started so we will know len
        char *json_start = p;


        // JSON body
        p = append_str(p, "{\"value1\":\"");
        p = append_ul(p, val);
        p = append_str(p, "\",\"value2\":\"");
        p = append_ul(p, millis());
        p = append_str(p, "\",\"value3\":\"");
        p = append_str(p, "MY NAME IS SKYNET!");
        p = append_str(p, "\"}");
      
        // go back and fill in the JSON length
        // we just know this is at most 2 digits (and need to fill in both)
        int i = strlen(json_start);
        content_length_here[0] = '0' + (i/10);
        content_length_here[1] = '0' + (i%10);
      
        // finally we are ready to send the POST to the server!
        client.print(post_rqst);
        Serial.print("Posting request: ");
        Serial.println(post_rqst);
        client.stop();    
      } else {
        Serial.println("Can't connect to IFTTT");
      }   
}

