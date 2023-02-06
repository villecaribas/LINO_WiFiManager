#include <Arduino.h>
#include <LINO_WiFi_Conexao.h>

bool ledState = 0;
const int ledPin = BUILTIN_LED;

AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta charset='utf-8' name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #519dd6;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #50b1d5;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #d62408;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <center><p><img src='data:image/png;base64,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'/></p></center>
  <div class="topnav">
    <h1>Teste de conexão</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="state">%STATE%</span></p>
      <p><button id="button" class="button">Inverter</button></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    if (event.data == "1"){
      state = "ON";
    }
    else{
      state = "OFF";
    }
    document.getElementById('state').innerHTML = state;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send('toggle');
  }
</script>
</body>
</html>
)rawliteral";

void notifyClients() {
    ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if (strcmp((char *)data, "toggle") == 0) {
            ledState = !ledState;
            notifyClients();
        }
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    webServer.addHandler(&ws);
}

String processor(const String &var) {
    Serial.println(var);
    if (var == "STATE") {
        if (ledState) {
            return "ON";
        } else {
            return "OFF";
        }
    }
    return String();
}

void setup() {
    Serial.begin(115200);

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    // put your setup code here, to run once:
    conectarWiFi();

    initWebSocket();

    // Route for root / web page
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html, processor);
    });

    // Start server
    webServer.begin();
}

void loop() {
    // put your main code here, to run repeatedly:
    drd->loop();
    ws.cleanupClients();
    digitalWrite(ledPin, ledState);
    check_WiFi();
}