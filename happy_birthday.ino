#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Konfigurasi OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Konfigurasi Buzzer & Lagu ---
const int speakerPin = 33; // PIN BUZZER: esp32 33. esp8266 D5.
const int songLength = 28;
const char notes[] = "GGAGcB GGAGdc GGxecBA yyecdc";
const int beats[] = {2,2,8,8,8,16,1,2,2,8,8,8,16,1,2,2,8,8,8,8,16,1,2,2,8,8,8,16};
const int tempo = 250; // tempo

// --- Konfigurasi Tombol Replay ---
const int buttonPin = 4; // 4 esp32 // D6 esp8266
volatile bool replaySequence = false;
                                     
// Variabel untuk debouncing tombol
int buttonState;
int lastButtonState = HIGH; // Asumsi tombol pull-up, tidak ditekan = HIGH
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // 50 ms

// --- Kustomisasi Pesan OLED ---
#define NICKNAME "name" // Ganti dengan nama panggilan
#define WISH_TEXT "Happy Birthday (name)! Wish you all the best! Semoga harapan dan keinginan kamu tercapai! Sukses selalu ya!"

// --- Variabel Global untuk Nada Buzzer ---
const char noteNames[] = {'C', 'D', 'E', 'F', 'G', 'A', 'B',
                          'c', 'd', 'e', 'f', 'g', 'a', 'b',
                          'x', 'y' };
const int noteTones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014,
                           956,  851,  758,  716,  638,  568,  506,
                           675,  700 };
const int TONE_SPEED_DIVISOR = 5;

// --- Struktur Lirik & Frasa Lagu ---
const char* lyrics_L1[] = { "Happy Birthday", "Happy Birthday", "Happy B'day dear", "Happy Birthday" };
const char* lyrics_L2_templates[] = { "to You!", "to You!", "%s!", "to You!" };
const int numPhrases = 4;
const int phraseStartIndices[] = {0,  7, 14, 22};
const int phraseEndIndices[]   = {6, 13, 21, 27};
char formattedLyricL2[32];

// --- Fungsi untuk Buzzer ---
void playTone(int tonePeriod, int durationMs) {
  if (tonePeriod <= 0) { delay(durationMs); return; }
  long totalDurationMicroseconds = (long)durationMs * 1000;
  for (long i = 0; i < totalDurationMicroseconds; i += tonePeriod * 2) {
    digitalWrite(speakerPin, HIGH); delayMicroseconds(tonePeriod);
    digitalWrite(speakerPin, LOW);  delayMicroseconds(tonePeriod);
  }
}

void playNote(char noteChar, int durationMs) {
  int tonePeriodToPlay = 0;
  for (int i = 0; i < (sizeof(noteNames)/sizeof(char)); i++) {
    if (noteNames[i] == noteChar) {
      tonePeriodToPlay = noteTones[i]; break;
    }
  }
  playTone(tonePeriodToPlay, durationMs / TONE_SPEED_DIVISOR);
}

// --- Fungsi untuk OLED ---
void displayCurrentLyricPhrase(const char* line1, const char* line2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  int16_t x1_b, y1_b; uint16_t w1, h1;
  display.getTextBounds(line1, 0, 0, &x1_b, &y1_b, &w1, &h1);
  int16_t cursorX1 = (SCREEN_WIDTH - w1) / 2; if (cursorX1 < 0) cursorX1 = 0;
  display.setCursor(cursorX1, 20); display.print(line1);

  int16_t x2_b, y2_b; uint16_t w2, h2;
  display.getTextBounds(line2, 0, 0, &x2_b, &y2_b, &w2, &h2);
  int16_t cursorX2 = (SCREEN_WIDTH - w2) / 2; if (cursorX2 < 0) cursorX2 = 0;
  display.setCursor(cursorX2, 20 + h1 + 4); display.print(line2);
  
  display.display();
}

void displayWishMessage(const char* wish) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  // Word wrap sederhana
  String wishString = wish; int16_t x = 0, y = 0; 
  char buffer[wishString.length() + 1]; wishString.toCharArray(buffer, wishString.length() + 1);
  char* Ptr = buffer; char* Str = Ptr;
  while (*Ptr != 0) {
    while (*Ptr != ' ' && *Ptr != 0) Ptr++; 
    char Temp = *Ptr; *Ptr = 0; 
    if (x + (strlen(Str) * 6) > SCREEN_WIDTH && x > 0) { 
        x = 0; y += 8; 
        if (y + 8 > SCREEN_HEIGHT) { display.display(); delay(1500); display.clearDisplay(); x=0; y=0; }
        display.setCursor(x,y);
    }
    display.print(Str); x += strlen(Str) * 6;
    if (Temp != 0) { 
      *Ptr = Temp;   
      if (Temp == ' ') {
          if (x + 6 <= SCREEN_WIDTH) { display.print(' '); x += 6; } 
          else { x = 0; y += 8;
            if (y + 8 > SCREEN_HEIGHT) { display.display(); delay(1500); display.clearDisplay(); x=0; y=0;}
            display.setCursor(x,y);
          } Ptr++;
      }
    } Str = Ptr; 
  }
  display.display();
}

// --- Fungsi Urutan Utama Ucapan ---
void runBirthdaySequence() {
  Serial.println(F("Memulai urutan ucapan..."));

  // Layar pembuka singkat
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  const char* title = "Happy";
  const char* title2 = "B'Day!";
  // Calculate positions for centered two-line display
  int16_t x1_b, y1_b; uint16_t w1, h1;
  display.getTextBounds(title, 0, 0, &x1_b, &y1_b, &w1, &h1);
  int16_t x2_b, y2_b; uint16_t w2, h2;
  display.getTextBounds(title2, 0, 0, &x2_b, &y2_b, &w2, &h2);
  // Display first line
  display.setCursor((SCREEN_WIDTH - w1) / 2, 16);
  display.print(title);
  // Display second line
  display.setCursor((SCREEN_WIDTH - w2) / 2, 16 + h1 + 4);
  display.print(title2);

  display.display();
  delay(2000);

  // Mainkan lagu dengan lirik yang sinkron
  for (int phrase = 0; phrase < numPhrases; phrase++) {
    if (phrase == 2) { // Frasa ke-3 untuk nickname
      snprintf(formattedLyricL2, sizeof(formattedLyricL2), lyrics_L2_templates[phrase], NICKNAME);
      displayCurrentLyricPhrase(lyrics_L1[phrase], formattedLyricL2);
    } else {
      displayCurrentLyricPhrase(lyrics_L1[phrase], lyrics_L2_templates[phrase]);
    }
    // delay(100); // Opsional: jeda singkat agar lirik terbaca sebelum musik frasa

    for (int i = phraseStartIndices[phrase]; i <= phraseEndIndices[phrase]; i++) {
      int currentBeatDuration = beats[i] * tempo;
      if (notes[i] == ' ') {
        delay(currentBeatDuration);
      } else {
        playNote(notes[i], currentBeatDuration);
      }
      if (i < songLength -1) { 
         delay(tempo / 3); 
      } else if (notes[i] != ' ' && i == songLength -1) {
         delay(tempo/3);
      }
    }
  }

  // Jeda sebelum pesan harapan
  delay(500);
  displayWishMessage(WISH_TEXT);

  // Tampilkan pesan untuk replay
  delay(8000); // Tampilkan pesan harapan selama beberapa detik
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 20); // Sesuaikan posisi
  display.println(F("Tekan tombol"));
  display.setCursor(15, 30);
  display.println(F("untuk putar ulang!"));
  display.display();
  Serial.println(F("Urutan selesai. Tunggu tombol..."));
}


// --- Program Utama ---
void setup() {
  Serial.begin(115200);
  pinMode(speakerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Set pin tombol sebagai INPUT dengan PULLUP internal

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 Gagal")); for(;;);
  }
  Serial.println(F("Sistem Siap. Tekan tombol untuk memulai atau replay."));
  
  // Jalankan urutan sekali saat startup
  runBirthdaySequence();
}

void loop() {
  int reading = digitalRead(buttonPin);

  // Jika ada perubahan state tombol (dari tidak ditekan ke ditekan atau sebaliknya)
  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // Catat waktu perubahan
  }

  // Setelah waktu debounce berlalu, cek lagi state tombol
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Jika state tombol berbeda dari state sebelumnya yang sudah stabil (artinya perubahan valid)
    if (reading != buttonState) {
      buttonState = reading; // Simpan state tombol yang baru dan stabil

      // Jika tombol ditekan (state menjadi LOW karena INPUT_PULLUP)
      if (buttonState == LOW) {
        Serial.println(F("Tombol ditekan! Memutar ulang..."));
        runBirthdaySequence(); // Panggil fungsi urutan ucapan lagi
      }
    }
  }
  lastButtonState = reading; // Simpan pembacaan tombol saat ini untuk iterasi berikutnya
}