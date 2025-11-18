#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <BLEDevice.h>
#include <time.h>

// =========================
// Cấu hình NTP / WiFi / Firebase
// =========================
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;  // GMT+7
const int daylightOffset_sec = 0;

const char* ssid = "Longnv";
const char* password = "12345678";

#define API_KEY "AIzaSyDE2YgJ_-kNWNmzI_8CMvBTRS3SV7V2Z0c"
#define DATABASE_URL "https://smartfan-e2acf-default-rtdb.asia-southeast1.firebasedatabase.app"

// =========================
// BLE / Protocol
// =========================
static BLEUUID SERVICE_UUID("0000FFE0-0000-1000-8000-00805F9B34FB");
static BLEUUID CHAR_UUID("0000FFE1-0000-1000-8000-00805F9B34FB");
const char* TARGET_NAME = "HMSoft";
const char* TARGET_UID = "d4:36:39:93:f7:2b";

enum : uint8_t { STX = 0x02,
                 ETX = 0x03 };

enum : uint8_t {
  TXRX_POWER = 0x01,
  TXRX_MODE = 0x02,
  TXRX_LEVEL = 0x03,
  TXRX_REQ_STATUS = 0x04,

  RX_ACK_POWER = 0x81,
  RX_ACK_MODE = 0x82,
  RX_ACK_LEVEL = 0x83,
  RX_STATUS = 0x84,
};

// Firebase objects
FirebaseData fbdo;
FirebaseConfig config;
FirebaseAuth auth;
bool signupOK = false;

// BLE objects
static BLEClient* Client = nullptr;
static BLERemoteCharacteristic* RemoteCharacteristic = nullptr;
static bool BleConnected = false;
static bool DoConnect = false;
static bool DoScan = true;
BLEAddress* TargetAddr = nullptr;
unsigned long LastScan = 0;
unsigned long LastStatusPoll = 0;

// Buffers
volatile bool newAck = false;
volatile bool newStatus = false;
uint8_t ackBuf = 0;
uint8_t errBuf = 0, modeBuf = 0, levelBuf = 0, tempBuf = 0;

// Throttle
static unsigned long lastNotify = 0;
static const unsigned long notifyMinInterval = 150;

static unsigned long lastFBWrite = 0;
static const unsigned long fbMinInterval = 300;

static void logHex(const uint8_t* d, size_t n) {
  for (size_t i = 0; i < n; ++i) Serial.printf("%02X ", d[i]);
}

// Gửi frame BLE
static void sendFrame(uint8_t id, uint8_t len, uint8_t val) {
  if (!BleConnected || !RemoteCharacteristic) {
    Serial.println("### LỖI: Firebase yêu cầu, nhưng BLE chưa kết nối.");
    return;
  }
  uint8_t f[5] = { STX, id, len, val, ETX };
  RemoteCharacteristic->writeValue(f, 5, false);
  Serial.print("TX: ");
  logHex(f, 5);
  Serial.println();
}

static void requestStatus() {
  if (!BleConnected || !RemoteCharacteristic) return;
  uint8_t f[4] = { STX, TXRX_REQ_STATUS, 0x00, ETX };
  RemoteCharacteristic->writeValue(f, 4, false);
  Serial.println("gui lenh doc trang thai");
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice d) override {
    String devName = d.haveName() ? d.getName().c_str() : "";
    String devAddr = d.getAddress().toString().c_str();
    if (devName == TARGET_NAME || devAddr.equalsIgnoreCase(TARGET_UID)) {
      if (TargetAddr) delete TargetAddr;
      TargetAddr = new BLEAddress(d.getAddress());
      DoConnect = true;
      DoScan = false;
      BLEDevice::getScan()->stop();
    }
  }
};

// BLE notify callback
static void notifyCb(BLERemoteCharacteristic*, uint8_t* p, size_t len, bool) {
  unsigned long now = millis();
  if (now - lastNotify < notifyMinInterval) return;
  lastNotify = now;

  Serial.print("BLE RX raw: ");
  logHex(p, len);
  Serial.println();

  if (len < 4 || p[0] != STX || p[len - 1] != ETX) return;

  uint8_t id = p[1];
  uint8_t l = p[2];

  if (id == RX_ACK_POWER || id == RX_ACK_MODE || id == RX_ACK_LEVEL) {
    ackBuf = p[3];
    newAck = true;
  } else if (id == RX_STATUS && l >= 4 && len >= 7) {
    errBuf = p[3];
    modeBuf = p[4];
    levelBuf = p[5];
    tempBuf = p[6];
    newStatus = true;
  }
}

static bool connectToServer(BLEAddress addr) {
  if (!Client) Client = BLEDevice::createClient();
  if (!Client) return false;

  Serial.printf("Thử kết nối đến: %s\n", addr.toString().c_str());

  if (!Client->connect(addr)) return false;

  auto* svc = Client->getService(SERVICE_UUID);
  if (!svc) {
    Client->disconnect();
    return false;
  }

  RemoteCharacteristic = svc->getCharacteristic(CHAR_UUID);
  if (!RemoteCharacteristic) {
    Client->disconnect();
    return false;
  }

  if (RemoteCharacteristic->canNotify())
    RemoteCharacteristic->registerForNotify(notifyCb);

  BleConnected = true;
  Serial.println(">>> ĐÃ KẾT NỐI BLE VỚI QUẠT <<<");
  return true;
}

static void bleDisconnectCleanup() {
  BleConnected = false;
  RemoteCharacteristic = nullptr;
  if (Client && Client->isConnected()) Client->disconnect();
}

// =========================
//      SETUP
// =========================
void setup() {
  Serial.begin(115200);
  delay(10);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println(" Connected!");

  // NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) delay(500);
  Serial.println("NTP OK.");

  // BLE init
  BLEDevice::init("");
  BLEScan* scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  scan->setActiveScan(true);
  scan->setInterval(160);
  scan->setWindow(120);

  // Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) signupOK = true;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase ready!");
}

// =========================
//        LOOP
// =========================
void loop() {
  unsigned long now = millis();

  // =========================
  // 1) POLLING Firebase command (thay cho stream!)
  // =========================
  static unsigned long lastCmdPoll = 0;
  if (now - lastCmdPoll > 300) {
    lastCmdPoll = now;

    if (Firebase.RTDB.getJSON(&fbdo, "/cmd")) {
      FirebaseJson& json = fbdo.jsonObject();
      FirebaseJsonData r;

      json.get(r, "cmd");

      if (r.success) {
        String cmd = r.to<String>();

        // Không xử lý nếu lệnh = none
        if (cmd != "none") {
          json.get(r, "val");
          uint8_t val = r.success ? r.to<int>() : 0;

          if (cmd == "power") sendFrame(TXRX_POWER, 1, val);
          else if (cmd == "mode") sendFrame(TXRX_MODE, 1, val);
          else if (cmd == "level") sendFrame(TXRX_LEVEL, 1, val);
          else if (cmd == "req_status") requestStatus();

          // Sau khi xử lý → reset lệnh để tránh loop
          Firebase.RTDB.setString(&fbdo, "/cmd/cmd", "none");
          Firebase.RTDB.setInt(&fbdo, "/cmd/val", 0);
        }
      }
    }
  }

  // =========================
  // 2) Ghi ACK + STATUS lên Firebase (rate-limited)
  // =========================
  if (newAck && now - lastFBWrite > fbMinInterval) {
    newAck = false;
    lastFBWrite = now;
    Firebase.RTDB.setInt(&fbdo, "/status/ack_code", ackBuf);
  }

  if (newStatus && now - lastFBWrite > fbMinInterval) {
    newStatus = false;
    lastFBWrite = now;

    Firebase.RTDB.setInt(&fbdo, "/status/error_code", errBuf);
    Firebase.RTDB.setInt(&fbdo, "/status/temperature", tempBuf);

    if (modeBuf == 3) {
      Firebase.RTDB.setBool(&fbdo, "/status/power", false);
    } else {
      Firebase.RTDB.setBool(&fbdo, "/status/power", true);
      Firebase.RTDB.setInt(&fbdo, "/status/mode", (modeBuf == 1) ? 1 : 0);
      Firebase.RTDB.setInt(&fbdo, "/status/level", levelBuf);
    }
  }

  // =========================
  // 3) BLE reconnect + scan
  // =========================
  if (DoConnect && TargetAddr) {
    DoConnect = false;
    if (!connectToServer(*TargetAddr)) DoScan = true;
    else requestStatus();
  }

  if (!BleConnected && DoScan && now - LastScan > 8000) {
    BLEDevice::getScan()->start(10, false);
    BLEDevice::getScan()->clearResults();
    LastScan = now;
  }

  if (BleConnected && Client && !Client->isConnected()) {
    bleDisconnectCleanup();
    DoScan = true;
  }

  delay(20);
}
