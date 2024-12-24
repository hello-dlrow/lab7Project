#include "buzzer.h"

extern const int buzzerPin;

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



// 音符和时长定义
int songNotes[] = {
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, 0, NOTE_B4, NOTE_A4
};

int songDurations[] = {
  4, 8, 8, 4, 4, 4, 4
};

// 控制变量
int noteIndex = 0;
unsigned long lastUpdate = 0;
int duration = 0;
bool playing = false;

// 播放音乐的函数
void play8BitTune() {
  unsigned long now = millis();

  // 如果当前没有播放音符并且仍有音符需要播放
  if (!playing && noteIndex < (sizeof(songNotes) / sizeof(songNotes[0]))) {
    // 计算音符的时长
    duration = (1000 / songDurations[noteIndex]) * 1.5; // 包括音符间隔
    lastUpdate = now;

    // 播放当前音符，如果是 0 则保持静音
    if (songNotes[noteIndex] != 0) {
      tone(buzzerPin, songNotes[noteIndex]);
    } else {
      noTone(buzzerPin);
    }

    playing = true; // 标记音符正在播放
  }

  // 如果音符播放完成
  if (playing && (now - lastUpdate >= duration)) {
    noTone(buzzerPin); // 停止音符
    noteIndex++;        // 跳到下一个音符
    playing = false;    // 重置播放状态
  }

  // 如果播放完成一轮，重置索引以循环播放
  if (noteIndex >= (sizeof(songNotes) / sizeof(songNotes[0]))) {
    noteIndex = 0;
  }
}


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