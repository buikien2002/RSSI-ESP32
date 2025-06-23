/*
B1: Thay đổi name, pass WiFi
B2: Nạp code, đặt thiết bị ở vị trí 1m so với nguồn phát và nhập "1" vào Serial
B3: Thay đổi đến vị trí khác, sau đó nhập khoảng cách khi đó, lúc đó sẽ chuyển sang chế độ đo khoảng cách
(B4): Nếu muốn set lại các giá trị thì nhấn 0 để làm lại
*/
#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include <ESP_Google_Sheet_Client.h>
#include <GS_SDHelper.h>
/* THAY ĐỔI NAME VÀ PASS WIFI */
// #define WIFI_NAME "vngalaxy2g"
// #define WIFI_PASS "vngalaxy2024"
#define WIFI_NAME "Kien"
#define WIFI_PASS "11052002"
#define SAMPLES 100

// Google Project ID
#define PROJECT_ID "rssi-data"

// Service Account's client email
#define CLIENT_EMAIL "rssi-data@rssi-data.iam.gserviceaccount.com"
// Service Account's private key
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQC4Kj7L7if8nGVR\nbqiy7ClRNJMah3kUSIBAxlNw5Z/dAnnMV6KF/odTRVNvzIwmwST9zlJ/7qG9QcaZ\nL7Caqmseg2q+JkHmojvWCvQaPv8jA+vxx2u7upXL9UAxTzydgTQ9uw/kuevMhxKD\nckUFJ+iZZ6sMYX551MC4VTCJu5i3LkJ9mre1u7MGg/eZAjAx/XFVUzbZBoBvVgeu\njsdQmJT4i2epDydS47hry/pJ+Tt5vNa/jMpEMq4T1JAN+ZfpzqiDTU+ikjWJH/fs\nzZERbL/fFvXHE2u3poQ3zVTps/3LUEGe3zvFCQsKcm3PXsSJvKFkd6u8FDRI6CAV\nbx92CJydAgMBAAECggEADddS9+WyyKgCb1/XOvdH1awTYDuNTIp0ZSnirIn9fVFJ\n0SNG+jYaepYrwqvHXsRGYPLoPWg0bLJsWCf+6GjlexoaZCcg7dIEmD4jgXN5nmqV\niYrY3HeV+Ym5pNsVLas/v2H3PFQC1lDSgeGNMWmB8EGb1+bd6A8m2tPAPaHFRQMK\n4FOBVwPr/UtTZ6wAAH6AfMJQjr+CzyFfM5wNC6EU7qlHS/pK7pPhV0Au5Ji/tq06\nDnvprnrtrALNdK57itC7bwjrTVa0Zx8dCdf3h96W0af94w3UCM1gCeg082vMApoM\nVdkGsdA/2HAOX5ne5+CJxodJorFFZaLwqfWnaWFDMQKBgQDhOMmRwW1BJE499ep+\nT6cJgaTZyyZU2NERo80Mrxh4vJ0geKzxh7tAwkv3EUw/nJeLta2s5aJoGgHW5kMh\nR33FMS+VprYiXJwHJwErOuTm0DpqXLGrq0ji/2b/5fBc9RtDd9AVdmIi+tksMgLA\nDNAQqEKhkgi12qisRWir6vALVQKBgQDRVR372p4RxY46K4EdrevOl70o5QUsTupL\nQUpwjtXCzKsnwdRTHShQ1JXpjrZFaVmKkPsK2JgCdtlTY9mwwH+0N9MgLZlNG8C3\nXn+9+BmTCSd2pj7ETa0iBjSm5MzvbWXGJ9Hr7ByELq85ri+8JxdbvmmuZ4kV3Ojs\n5GicmfOcKQKBgQCirgwa9iZeiUTiyICKoCo9WaGjl3Omkb3iCfkQkmEQxNySGdiB\n1dLR4gZQ/fOfdxKX3bEHVA9NNovhmA0hOqyEBRUj+XlRtKcXG3tiqHWyqvGm5jGH\nmH9bR7Q0XviRjJIW17I77njQX3UC0VTfN+IKRh9q6TrVcI2pcbE7jMW6yQKBgG8P\n/OeJ9wGw0iQUvt1TUF/869SNpe/7Mp2Y13jen5VJ56khUpiCSMiR8bPIDvbGzUYS\nY88+WXA9RFLVEEOR7m9civxwlPxvNBFV5uqQfmkfmzt9OqIGIEyEBF1j2UNSxIOc\nzuigDgUni7pLq3HlP6RiY0mwDvB1k3NUQBiOKcz5AoGAWUSTyuf3j1czEHU75tJz\nVbYZFwwz0HUixCvvWA81WZXcCJEPpaJbsh/5/bIQ08FaJERcAs+VjRhW3GTg4+zD\nu1bnVo92GFtlceNO7seN2o/P8P+6tmDbbGNIzbI8vXoE9pGm6JEfpCiENnPTROOM\ncIRg/uqetCPYfXPJwyyAYmw=\n-----END PRIVATE KEY-----\n";

// The ID of the spreadsheet where you'll publish the data
const char spreadsheetId[] = "1f71n3D2-1jqdruKPMtylm5CIa19Ah_HhCBGexPqay80";

float measuredPower = 0; // RSSI ở 1m
float n = 0;             // Giá trị suy hao môi trường
int mode = 1;            // Bắt đầu với mode 1

float rssiAvg = 0; // Giá trị RSSI trung bình
float distanceAvg = 0; // Khoảng cách trung bình
int number_row = 1;

const char *ntpServer = "pool.ntp.org";

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  
  configTime(0, 0, ntpServer);
  GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION);
  
  // Connect to Wi-Fi
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_NAME, WIFI_PASS);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      delay(1000);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  GSheet.setTokenCallback(tokenStatusCallback);
  GSheet.setPrerefreshSeconds(10 * 60);
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
}

void loop()
{
  rssiAvg = measureRSSI();
  bool ready = GSheet.ready();

  if (mode == 1)
  {
    Serial.println("Mode 1 (Read RSSI): " + String(rssiAvg) + " dBm");
    Serial.println("Put '1' to get measuredPower value");
  }
  else if (mode == 2)
  {
    Serial.println("Mode 2 (Read RSSI): " + String(rssiAvg) + " dBm");
    Serial.println("Put other distance to get n value");
  }
  else if (mode == 3)
  {
    distanceAvg = calculateDistance(measuredPower, rssiAvg, n);
    Serial.println("Mode 3 (Read distance): " + String(distanceAvg) + " m");
    Serial.println("Put '0' to change to Mode 1");

    if (ready)
    {
      FirebaseJson response;
      FirebaseJson valueRange;

      valueRange.add("majorDimension", "COLUMNS");
      valueRange.set("values/[0]/[0]", number_row);
      valueRange.set("values/[1]/[0]", rssiAvg);
      valueRange.set("values/[2]/[0]", distanceAvg);

      number_row++;

      bool success = GSheet.values.append(&response, spreadsheetId, "Sheet1!A1", &valueRange);
      if (success)
      {
          response.toString(Serial, true);
          valueRange.clear();
      }
      else
      {
          Serial.println(GSheet.errorReason());
      }
    }
  }

  if (Serial.available())
  {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (mode == 1 && input == "1")
    {
      measuredPower = rssiAvg;
      Serial.print("=> Save measuredPower = ");
      Serial.println(measuredPower);
      mode = 2;
    }
    else if (mode == 2)
    {
      float d = input.toFloat();
      if (d > 0 && d != 1)
      {
        n = calculateN(measuredPower, rssiAvg, d);
        Serial.print("=> Save n = ");
        Serial.println(n);
        mode = 3;
      }
      else
      {
        Serial.println("Put again !!!");
      }
    }
    else if (mode == 3 && input == "0")
    {
      measuredPower = 0;
      n = 0;
      mode = 1;
      Serial.println("=> Reset to Mode 1");
    }
  }
  Serial.println("--------------------------------------------------------");
  delay(1000);
}

void tokenStatusCallback(TokenInfo info)
{
  if (info.status == token_status_error)
  {
    GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
  }
  else
  {
    GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
  }
}

// Hàm đo RSSI trung bình
float measureRSSI()
{
  long rssiSum = 0;

  for (int i = 0; i < SAMPLES; i++)
  {
    rssiSum += WiFi.RSSI();
    delay(10);
  }

  return (float)rssiSum / SAMPLES;
}

// Hàm tính n từ measuredPower và RSSI tại khoảng cách d
float calculateN(float measuredPower, float rssiAtD, float d)
{
  return (measuredPower - rssiAtD) / (10 * log10(d));
}

// Hàm tính khoảng cách từ RSSI
float calculateDistance(float measuredPower, float rssi, float n)
{
  return pow(10.0, (measuredPower - rssi) / (10 * n));
}