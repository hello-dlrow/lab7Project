#include "buzzer.h"

int melody[] = {
  NOTE_E5, NOTE_E5, 0, NOTE_E5,
  0, NOTE_C5, NOTE_E5, 0,
  NOTE_G5, 0, 0,  0,
  NOTE_G4, 0, 0, 0,

  NOTE_C5, 0, 0, NOTE_G4,
  0, 0, NOTE_E4, 0,
  0, NOTE_A4, 0, NOTE_B4,
  0, NOTE_AS4, NOTE_A4, 0,

  NOTE_G4, NOTE_E5, NOTE_G5,
  NOTE_A5, 0, NOTE_F5, NOTE_G5,
  0, NOTE_E5, 0, NOTE_C5,
  NOTE_D5, NOTE_B4, 0, 0
};

const int melodyLength = sizeof(melody) / sizeof(melody[0]);
// 控制变量
int currentNote = 0;
unsigned long previousMillis = 0;
int noteDuration = 0;
bool isNotePlaying = false;

// 定义音符时长（以四分音符为基准，数字越大音符越短）
int noteDurations[] = {
  8, 8, 8, 8,
  8, 8, 8, 8,
  4, 8, 8, 8,
  4, 8, 8, 8,

  4, 8, 8, 8,
  4, 8, 8, 8,
  8, 8, 8, 8,
  4, 8, 8, 8,

  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  4, 8, 8, 8
};


void playMario() {
  unsigned long currentMillis = millis();

  if (!isNotePlaying && currentNote < melodyLength) {
    // 开始播放新音符
    noteDuration = (1000 / noteDurations[currentNote]) * 1.3; // 包含间隔
    previousMillis = currentMillis;
    if (melody[currentNote] != 0) {
      tone(buzzerPin, melody[currentNote]);
    } else {
      noTone(buzzerPin);
    }
    isNotePlaying = true;
  }

  if (isNotePlaying && (currentMillis - previousMillis >= noteDuration)) {
    // 停止当前音符，准备下一个
    noTone(buzzerPin);
    currentNote++;
    isNotePlaying = false;
  }

  if (currentNote >= melodyLength) {
    // 旋律播放完毕，重置索引以循环播放
    currentNote = 0;
  }
}