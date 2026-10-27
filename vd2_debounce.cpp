/*
 * BAI 2 - VI DU 2: CHONG DOI (DEBOUNCE) BANG millis()
 * ---------------------------------------------------------------
 * Mach giu nguyen bai 1: nut nhan GPIO4 (INPUT_PULLUP), LED GPIO2.
 *
 * Nguyen tac chong doi (giong so do):
 *   - Suon xuong DAU TIEN -> ISR dem 1, dong thoi ghi lai moc thoi gian
 *     (lastInterruptTime = millis()).
 *   - MOI suon xuong xay ra trong vong DEBOUNCE_MS (50ms) ke tu suon dau
 *     -> bi BO QUA (khong dem), vi do la nhieu/nay cua tiep diem co khi,
 *     khong phai mot lan nhan moi.
 *   - Sau khi het 50ms, ISR san sang nhan suon xuong tiep theo (lan nhan
 *     moi thuc su).
 *
 * Ket qua: moi lan nhan nut DUT KHOAT chi lam debouncedCount tang DUNG 1,
 * khac han vi du 1 (isrCount tang nhieu don vi vi dinh nay).
 *
 * Cach quan sat bang do thi (plot) trong VS Code voi extension Teleplot:
 *   1. Cai extension "Teleplot" (alexnesnes.teleplot) trong VS Code.
 *   2. Nap chuong trinh nay vao board (env vd2_debounce).
 *   3. Mo Serial Monitor cua VS Code, tro toi COM port cua ESP32, baud
 *      115200 -> Teleplot se tu dong ve do thi tu cac dong ">ten:gia_tri".
 *   4. Nhan nut nhieu lan, so sanh debouncedCount (vi du nay) voi isrCount
 *      (vi du 1) de thay ro hieu qua cua chong doi.
 */

#include <Arduino.h>

#define BUTTON_PIN    4     // Chan nut nhan (ngat ngoai)
#define LED_PIN       2     // Chan LED bao hieu
#define DEBOUNCE_MS   50    // Cua so chong doi: bo qua suon xuong trong 50ms sau suon dau

volatile uint32_t debouncedCount = 0;        // So lan nhan nut THUC SU (da chong doi)
volatile uint32_t rawEdgeCount = 0;          // So suon xuong THO (de doi chieu, chua chong doi)
volatile unsigned long lastInterruptTime = 0; // Moc thoi gian (ms) cua suon xuong hop le gan nhat

// ----------------- ISR: CO CHONG DOI BANG millis() -----------------
void IRAM_ATTR handleInterrupt() {
  rawEdgeCount++;   // Ghi nhan moi suon xuong tho, chi de doi chieu/giai thich

  unsigned long now = millis();
  if (now - lastInterruptTime >= DEBOUNCE_MS) {
    // Da qua 50ms ke tu suon xuong hop le truoc do -> day la lan nhan MOI
    debouncedCount++;
    lastInterruptTime = now;
  }
  // Nguoc lai: suon xuong nay roi vao "cua so chong doi" -> bo qua, khong dem
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("=== BAI 2 - VD2: Chong doi (debounce) bang millis(), cua so 50ms ===");
  Serial.println("Nhan nut nhieu lan va so sanh debouncedCount voi rawEdgeCount.");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleInterrupt, FALLING);
}

void loop() {
  int pinState = digitalRead(BUTTON_PIN);
  digitalWrite(LED_PIN, pinState == LOW ? HIGH : LOW);

  // Dinh dang Teleplot: moi dong ">ten:gia_tri" ve 1 duong rieng tren do thi.
  // pinState:       dang song tho tren chan GPIO4 (van con nay nhu vd1)
  // rawEdgeCount:   tong so suon xuong tho (se tang nhieu/lan nhan, giong vd1)
  // debouncedCount: so lan nhan da duoc chong doi (chi tang 1/lan nhan that)
  Serial.print(">pinState:");
  Serial.println(pinState);
  Serial.print(">rawEdgeCount:");
  Serial.println(rawEdgeCount);
  Serial.print(">debouncedCount:");
  Serial.println(debouncedCount);

  delay(2);
}
