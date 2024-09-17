
#include <WiFi.h>
int ledPin = 2;

char ch;

//For letters
char* letters[] = {
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R 
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." // S-Z
};

//For Numbers
char* numbers[] = {
  "-----", ".----", "..---", "...--", "....-", ".....",
"-....", "--...", "---..", "----."
};

int dotDelay = 200;

// Replace with your network credentials
const char* ssid     = "ESP32-Morse-AP";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 2;
const int output27 = 27;

void setup() {
  Serial.begin(9600);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();

  
  server.begin();
  

  
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              //Serial.println("GPIO 26 on");

              Serial.println("TEST");
             // output26State = "on";
             // digitalWrite(output26, HIGH);
char str[]="Hello world";
for(int i=0;str[i];i++){
    Serial.println(str[i]);

// ch = 'a'; // read a single letter if (ch >= 'a' && ch <= 'z')
if (str[i] >= 'a' && str[i] <= 'z')
{
flashSequence(letters[str[i] - 'a']);
}
else if (str[i] >= 'A' && str[i] <= 'Z') {
flashSequence(letters[str[i] - 'A']); }
else if (str[i] >= '0' && str[i] <= '9') {
flashSequence(numbers[str[i] - '0']); }
else if (str[i] == ' ') {
delay(dotDelay * 4);
}
}

            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              //Serial.println("GPIO 26 on");

              Serial.println("TEST");
             // output26State = "on";
             // digitalWrite(output26, HIGH);
char str[]="KK7BGO";
for(int i=0;str[i];i++){
    Serial.println(str[i]);

// ch = 'a'; // read a single letter if (ch >= 'a' && ch <= 'z')
if (str[i] >= 'a' && str[i] <= 'z')
{
flashSequence(letters[str[i] - 'a']);
}
else if (str[i] >= 'A' && str[i] <= 'Z') {
flashSequence(letters[str[i] - 'A']); }
else if (str[i] >= '0' && str[i] <= '9') {
flashSequence(numbers[str[i] - '0']); }
else if (str[i] == ' ') {
delay(dotDelay * 4);
}
}
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Morse Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">Hello World</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">KK7BGO</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();

  }
 //Addition Start 

if (Serial.available())
{
ch = Serial.read(); // read a single letter if (ch >= 'a' && ch <= 'z')
if (ch >= 'a' && ch <= 'z')
{
flashSequence(letters[ch - 'a']);
}
else if (ch >= 'A' && ch <= 'Z') {
flashSequence(letters[ch - 'A']); }
else if (ch >= '0' && ch <= '9') {
flashSequence(numbers[ch - '0']); }
else if (ch == ' ') {
delay(dotDelay * 4);
}
}
//Addition End 
}

void flashSequence(char* sequence) {
int i = 0;
while (sequence[i] != NULL) {
        flashDotOrDash(sequence[i]);
i++; }
delay(dotDelay * 3);
}


void flashDotOrDash(char dotOrDash) {
digitalWrite(output26, HIGH); if (dotOrDash == '.')
{
    delay(dotDelay);
  }
else // must be a - 
{
delay(dotDelay * 3); }
digitalWrite(output26, LOW); delay(dotDelay);
}
