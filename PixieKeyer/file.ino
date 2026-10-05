#include <WiFi.h>
#include <WebServer.h>

// ==================================================
// CONFIGURATION
// ==================================================

#define CW_PIN 26

const char* ssid = "Pixie-CW";
const char* password = "pixiecw123";

WebServer server(80);

int currentWPM = 15;


// ==================================================
// WEB PAGE
// ==================================================

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>

<meta charset="UTF-8">

<meta name="viewport"
      content="width=device-width, initial-scale=1">

<title>Pixie CW</title>

<style>

body {
  margin: 0;
  padding: 20px;
  background: #111;
  color: #fff;
  font-family: Arial, sans-serif;
  text-align: center;
}

.container {
  max-width: 650px;
  margin: auto;
}

h1 {
  color: #55dd88;
  margin-bottom: 5px;
}

h2 {
  margin-top: 35px;
  color: #ddd;
}

.subtitle {
  color: #999;
  margin-top: 0;
}

textarea {
  width: 95%;
  max-width: 550px;
  height: 120px;
  box-sizing: border-box;

  padding: 12px;

  font-size: 21px;

  border-radius: 8px;
  border: 1px solid #555;

  background: #222;
  color: white;
}

select {
  font-size: 19px;
  padding: 8px;
  border-radius: 6px;
}

button {
  font-size: 18px;
  padding: 13px 18px;

  margin: 6px;

  border: 0;
  border-radius: 8px;

  cursor: pointer;
}

.send {
  background: #28a745;
  color: white;
}

.save {
  background: #e0a800;
  color: #111;
}

.test {
  background: #007bff;
  color: white;
}

.load {
  background: #555;
  color: white;
}

.delete {
  background: #dc3545;
  color: white;
}

.quick {
  background: #1d1d1d;

  border: 1px solid #55dd88;

  padding: 16px;

  margin: 12px auto;

  border-radius: 10px;

  max-width: 550px;
}

.savedMessage {
  background: #222;

  border: 1px solid #444;

  padding: 14px;

  margin: 12px auto;

  border-radius: 8px;

  max-width: 550px;
}

.messageText {
  font-family: monospace;
  font-size: 18px;

  word-wrap: break-word;

  margin-bottom: 10px;
}

#status {
  margin: 20px;

  padding: 10px;

  font-size: 18px;

  min-height: 25px;
}

.ready {
  color: #55dd88;
}

.transmitting {
  color: #ffcc00;
}

.error {
  color: #ff6666;
}

.small {
  color: #888;
  font-size: 13px;
}

</style>

</head>


<body>

<div class="container">

<h1>Pixie CW</h1>

<p class="subtitle">
ESP32 CW Controller
</p>


<!-- ============================================= -->
<!-- MESSAGE ENTRY -->
<!-- ============================================= -->

<h2>Message</h2>

<textarea id="message"
          placeholder="Enter CW message...">CQ CQ CQ DE CALLSIGN CALLSIGN K</textarea>


<p>

Speed:

<select id="wpm">

<option value="5">5 WPM</option>
<option value="10">10 WPM</option>
<option value="12">12 WPM</option>

<option value="15" selected>
15 WPM
</option>

<option value="18">18 WPM</option>
<option value="20">20 WPM</option>
<option value="25">25 WPM</option>
<option value="30">30 WPM</option>

</select>

</p>


<button class="send"
        onclick="transmitCurrent()">

Transmit CW

</button>


<button class="save"
        onclick="saveCurrentMessage()">

Save Message

</button>


<br>


<button class="test"
        onclick="testGPIO()">

Test GPIO 26

</button>


<div id="status"
     class="ready">

Ready

</div>



<!-- ============================================= -->
<!-- STATIC QUICK MESSAGES -->
<!-- ============================================= -->

<h2>Quick Messages</h2>


<div class="quick">

<div class="messageText">

CQ CQ CQ DE CALLSIGN CALLSIGN K

</div>


<button class="send"
        onclick="transmitText(
          'CQ CQ CQ DE CALlSIGN CALLSIGN K'
        )">

SEND CQ

</button>

</div>



<!-- ============================================= -->
<!-- SAVED MESSAGES -->
<!-- ============================================= -->

<h2>Saved Messages</h2>


<div id="savedMessages">
</div>


<p class="small">

Saved messages are stored on this browser/device.

</p>


<p class="small">

CW Output: GPIO 26

</p>


</div>


<script>

// ==================================================
// LOCAL STORAGE
// ==================================================

const STORAGE_KEY =
  "pixieCWMessages";


// --------------------------------------------------
// GET SAVED MESSAGES
// --------------------------------------------------

function getSavedMessages() {

  try {

    const data =
      localStorage.getItem(
        STORAGE_KEY
      );


    if (!data) {

      return [];

    }


    return JSON.parse(data);

  }

  catch (error) {

    console.log(error);

    return [];

  }

}


// --------------------------------------------------
// STORE MESSAGES
// --------------------------------------------------

function storeMessages(messages) {

  localStorage.setItem(
    STORAGE_KEY,
    JSON.stringify(messages)
  );

}


// ==================================================
// STATUS
// ==================================================

function setStatus(
  text,
  state = "ready"
) {

  const status =
    document.getElementById(
      "status"
    );


  status.textContent =
    text;


  status.className =
    state;

}


// ==================================================
// SAVE MESSAGE
// ==================================================

function saveCurrentMessage() {

  const box =
    document.getElementById(
      "message"
    );


  const text =
    box.value.trim();


  if (!text) {

    setStatus(
      "Enter a message first.",
      "error"
    );

    return;

  }


  let messages =
    getSavedMessages();


  if (
    messages.includes(text)
  ) {

    setStatus(
      "Message is already saved.",
      "error"
    );

    return;

  }


  messages.push(text);


  storeMessages(
    messages
  );


  renderSavedMessages();


  setStatus(
    "Message saved."
  );

}


// ==================================================
// DELETE MESSAGE
// ==================================================

function deleteMessage(index) {

  let messages =
    getSavedMessages();


  messages.splice(
    index,
    1
  );


  storeMessages(
    messages
  );


  renderSavedMessages();


  setStatus(
    "Message deleted."
  );

}


// ==================================================
// LOAD MESSAGE
// ==================================================

function loadMessage(index) {

  const messages =
    getSavedMessages();


  if (
    typeof messages[index]
    === "undefined"
  ) {

    return;

  }


  document
    .getElementById("message")
    .value =
      messages[index];


  window.scrollTo({
    top: 0,
    behavior: "smooth"
  });


  setStatus(
    "Message loaded."
  );

}


// ==================================================
// SEND SAVED MESSAGE
// ==================================================

function sendSavedMessage(index) {

  const messages =
    getSavedMessages();


  if (
    typeof messages[index]
    === "undefined"
  ) {

    return;

  }


  transmitText(
    messages[index]
  );

}


// ==================================================
// DISPLAY SAVED MESSAGES
// ==================================================

function renderSavedMessages() {

  const container =
    document.getElementById(
      "savedMessages"
    );


  const messages =
    getSavedMessages();


  container.innerHTML =
    "";


  if (
    messages.length === 0
  ) {

    container.innerHTML =
      '<p class="small">' +
      'No saved messages yet.' +
      '</p>';

    return;

  }


  messages.forEach(
    function(message, index) {

      const card =
        document.createElement(
          "div"
        );


      card.className =
        "savedMessage";


      // MESSAGE TEXT

      const text =
        document.createElement(
          "div"
        );


      text.className =
        "messageText";


      text.textContent =
        message;


      // SEND BUTTON

      const send =
        document.createElement(
          "button"
        );


      send.className =
        "send";


      send.textContent =
        "SEND";


      send.onclick =
        function() {

          sendSavedMessage(
            index
          );

        };


      // LOAD BUTTON

      const load =
        document.createElement(
          "button"
        );


      load.className =
        "load";


      load.textContent =
        "LOAD / EDIT";


      load.onclick =
        function() {

          loadMessage(
            index
          );

        };


      // DELETE BUTTON

      const remove =
        document.createElement(
          "button"
        );


      remove.className =
        "delete";


      remove.textContent =
        "DELETE";


      remove.onclick =
        function() {

          if (
            confirm(
              "Delete this saved message?"
            )
          ) {

            deleteMessage(
              index
            );

          }

        };


      card.appendChild(
        text
      );


      card.appendChild(
        send
      );


      card.appendChild(
        load
      );


      card.appendChild(
        remove
      );


      container.appendChild(
        card
      );

    }
  );

}


// ==================================================
// TRANSMIT CURRENT MESSAGE
// ==================================================

function transmitCurrent() {

  const text =
    document
      .getElementById("message")
      .value
      .trim();


  if (!text) {

    setStatus(
      "Enter a message first.",
      "error"
    );

    return;

  }


  transmitText(
    text
  );

}


// ==================================================
// TRANSMIT ANY MESSAGE
// ==================================================

function transmitText(text) {

  const wpm =
    document
      .getElementById("wpm")
      .value;


  setStatus(
    "Transmitting: " + text,
    "transmitting"
  );


  fetch(
    "/send?text=" +
    encodeURIComponent(text) +
    "&wpm=" +
    encodeURIComponent(wpm)
  )

  .then(
    response =>
      response.text()
  )

  .then(
    data => {

      setStatus(
        data
      );

    }
  )

  .catch(
    error => {

      setStatus(
        "ESP32 connection error.",
        "error"
      );

    }
  );

}


// ==================================================
// TEST GPIO
// ==================================================

function testGPIO() {

  setStatus(
    "Testing GPIO 26...",
    "transmitting"
  );


  fetch("/test")

  .then(
    response =>
      response.text()
  )

  .then(
    data => {

      setStatus(
        data
      );

    }
  )

  .catch(
    error => {

      setStatus(
        "ESP32 connection error.",
        "error"
      );

    }
  );

}


// ==================================================
// INITIALIZE PAGE
// ==================================================

renderSavedMessages();

</script>


</body>

</html>
)rawliteral";


// ==================================================
// MORSE LOOKUP
// ==================================================

String getMorse(char c) {

  c = toupper(c);

  switch (c) {

    case 'A': return ".-";
    case 'B': return "-...";
    case 'C': return "-.-.";
    case 'D': return "-..";
    case 'E': return ".";

    case 'F': return "..-.";
    case 'G': return "--.";
    case 'H': return "....";
    case 'I': return "..";
    case 'J': return ".---";

    case 'K': return "-.-";
    case 'L': return ".-..";
    case 'M': return "--";
    case 'N': return "-.";
    case 'O': return "---";

    case 'P': return ".--.";
    case 'Q': return "--.-";
    case 'R': return ".-.";
    case 'S': return "...";
    case 'T': return "-";

    case 'U': return "..-";
    case 'V': return "...-";
    case 'W': return ".--";
    case 'X': return "-..-";
    case 'Y': return "-.--";

    case 'Z': return "--..";


    // NUMBERS

    case '0': return "-----";
    case '1': return ".----";
    case '2': return "..---";
    case '3': return "...--";
    case '4': return "....-";

    case '5': return ".....";
    case '6': return "-....";
    case '7': return "--...";
    case '8': return "---..";
    case '9': return "----.";


    // BASIC PUNCTUATION

    case '.': return ".-.-.-";
    case ',': return "--..--";
    case '?': return "..--..";
    case '/': return "-..-.";
    case '=': return "-...-";
    case '+': return ".-.-.";
    case '-': return "-....-";


    default:

      return "";

  }

}


// ==================================================
// CW TIMING
// ==================================================

int ditTime() {

  return 1200 /
         currentWPM;

}


// ==================================================
// KEY DOWN
// ==================================================

void keyDown(
  unsigned long duration
) {

  digitalWrite(
    CW_PIN,
    HIGH
  );


  delay(
    duration
  );


  digitalWrite(
    CW_PIN,
    LOW
  );

}


// ==================================================
// SEND ONE CHARACTER
// ==================================================

void sendCharacter(
  char character
) {

  String code =
    getMorse(
      character
    );


  if (
    code.length() == 0
  ) {

    return;

  }


  for (
    unsigned int i = 0;
    i < code.length();
    i++
  ) {

    if (
      code[i] == '.'
    ) {

      keyDown(
        ditTime()
      );

    }


    else if (
      code[i] == '-'
    ) {

      keyDown(
        ditTime() * 3
      );

    }


    // One dit between elements

    if (
      i <
      code.length() - 1
    ) {

      delay(
        ditTime()
      );

    }

  }

}


// ==================================================
// TRANSMIT CW MESSAGE
// ==================================================

void transmitCW(
  String text
) {

  // Safety:
  // Always begin released.

  digitalWrite(
    CW_PIN,
    LOW
  );


  text.toUpperCase();


  for (
    unsigned int i = 0;
    i < text.length();
    i++
  ) {

    char c =
      text[i];


    // ------------------------------
    // WORD SPACE
    // ------------------------------

    if (
      c == ' '
    ) {

      /*
        Previous character was
        followed by a 3-dit gap.

        Add 4 dits to produce
        standard 7-dit word spacing.
      */

      delay(
        ditTime() * 4
      );

      continue;

    }


    // ------------------------------
    // CHARACTER
    // ------------------------------

    sendCharacter(
      c
    );


    /*
      Standard spacing between
      characters = 3 dits.

      Only add it if there is
      another character coming.
    */

    if (
      i <
      text.length() - 1
    ) {

      delay(
        ditTime() * 3
      );

    }

  }


  // CRITICAL:
  // Key released after transmission.

  digitalWrite(
    CW_PIN,
    LOW
  );

}


// ==================================================
// ROOT WEB PAGE
// ==================================================

void handleRoot() {

  server.send(
    200,
    "text/html",
    webpage
  );

}


// ==================================================
// GPIO TEST
// ==================================================

void handleTest() {

  // Key ON

  digitalWrite(
    CW_PIN,
    HIGH
  );


  delay(
    1000
  );


  // Key OFF

  digitalWrite(
    CW_PIN,
    LOW
  );


  server.send(
    200,
    "text/plain",
    "GPIO 26 test complete"
  );

}


// ==================================================
// SEND REQUEST
// ==================================================

void handleSend() {

  if (
    !server.hasArg("text")
  ) {

    server.send(
      400,
      "text/plain",
      "No message entered"
    );

    return;

  }


  String text =
    server.arg(
      "text"
    );


  // ------------------------------
  // WPM
  // ------------------------------

  if (
    server.hasArg("wpm")
  ) {

    currentWPM =
      server
        .arg("wpm")
        .toInt();


    if (
      currentWPM < 5
    ) {

      currentWPM = 5;

    }


    if (
      currentWPM > 30
    ) {

      currentWPM = 30;

    }

  }


  Serial.print(
    "TX: "
  );

  Serial.println(
    text
  );


  Serial.print(
    "WPM: "
  );

  Serial.println(
    currentWPM
  );


  /*
    Respond to browser first.

    Then perform the CW transmission.
  */

  server.send(
    200,
    "text/plain",
    "CW transmission started"
  );


  transmitCW(
    text
  );


  Serial.println(
    "Transmission complete."
  );

}


// ==================================================
// SETUP
// ==================================================

void setup() {

  // -----------------------------------------------
  // GPIO FIRST
  // -----------------------------------------------

  pinMode(
    CW_PIN,
    OUTPUT
  );


  digitalWrite(
    CW_PIN,
    LOW
  );


  delay(
    100
  );


  // -----------------------------------------------
  // SERIAL
  // -----------------------------------------------

  Serial.begin(
    115200
  );


  Serial.println();
  Serial.println(
    "=============================="
  );

  Serial.println(
    "Pixie CW Controller"
  );

  Serial.println(
    "=============================="
  );


  Serial.print(
    "CW GPIO: "
  );

  Serial.println(
    CW_PIN
  );


  // -----------------------------------------------
  // WIFI ACCESS POINT
  // -----------------------------------------------

  digitalWrite(
    CW_PIN,
    LOW
  );


  WiFi.mode(
    WIFI_AP
  );


  digitalWrite(
    CW_PIN,
    LOW
  );


  WiFi.softAP(
    ssid,
    password
  );


  digitalWrite(
    CW_PIN,
    LOW
  );


  Serial.print(
    "WiFi: "
  );

  Serial.println(
    ssid
  );


  Serial.print(
    "Password: "
  );

  Serial.println(
    password
  );


  Serial.print(
    "Open: http://"
  );

  Serial.println(
    WiFi.softAPIP()
  );


  // -----------------------------------------------
  // WEB ROUTES
  // -----------------------------------------------

  server.on(
    "/",
    handleRoot
  );


  server.on(
    "/test",
    handleTest
  );


  server.on(
    "/send",
    handleSend
  );


  server.begin();


  // Final safety

  digitalWrite(
    CW_PIN,
    LOW
  );


  Serial.println(
    "Web server ready."
  );

}


// ==================================================
// LOOP
// ==================================================

void loop() {

  server.handleClient();

}
