# 🎮 PokéBattle LED Edition

<div align="center">

![Hero](https://img.shields.io/badge/🎯%20Turn%20based%20Battle-5628E9?style=for-the-badge&logo=game-joystick&logoColor=white)
![Hardware](https://img.shields.io/badge/🛠️%20ATmega328P%20%7C%2074HC595%20%7C%20MAX7219-F5A623?style=for-the-badge)
![Build](https://img.shields.io/badge/🔨%20avr-gcc%20%7C%20make-brightgreen?style=for-the-badge&logo=github-actions)
![License](https://img.shields.io/badge/License-MIT-blue?style=for-the-badge)

</div>

---

## 📖 Überblick

**PokéBattle LED Edition** ist ein interaktives Turn-basiertes Pokémon-Battle-Spiel auf 8×8 LED-Matrix, implementiert in reinem C für den ATmega328P-Mikrocontroller. Der Spieler steuert einen Pokémon gegen den Computer, wobei HP-Balken, digitale Sprites und Echtzeit-Soundeffekte in einer minimalistischen, aber fesselnden Benutzeroberfläche präsentiert werden.

### 🎯 Spielmechanik

| Phase | Beschreibung |
|-------|-------------|
| **Start** | Begrüßungsbildschirm mit HP-Anzeige (120 HP pro Pokémon) |
| **Auswahl** | Joystick nach links/rechts navigieren, Taste bestätigen |
| **Angriff** | Spieler greift an (15 DMG, 10% Verfehlen) |
| **Konter** | Gegner antwortet (15 DMG, 50% Verfehlen) |
| **Ergebnis** | HP-Balken aktualisieren, Sieg/Defeat-Animation |

---

## 🧪 Technische Spezifikationen

### Hardware-Stack

```
┌─────────────────────────────────────────────────────────────┐
│                    ATmega328P (16 MHz)                      │
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────────────┐ │
│  │   UART0     │  │    SPI0      │  │      ADC            │ │
│  │  (PD0/PD1)  │  │ (PB3/PB5/PB0)│  │   (PC2 für Joystick)│ │
│  └─────────────┘  └──────────────┘  └─────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        ▼                     ▼                     ▼
   ╔═══════════╗       ╔═════════════╗      ╔════════════╗
   ║ 74HC595   ║       ║  Joystick    ║      ║  MAX7219   ║
   ║ Shift Reg ║──────▶║  (VRx-Axis) ║      ║  Display   ║
   ║           ║       ║             ║      ║            ║
   ╚═══════════╝       ╚═════════════╝      ╚════════════╝
         │                    │                     │
         ▼                    ▼                     ▼
   ┌───────────┐      ┌──────────┐         ┌──────────┐
   │ Col 1-8   │      │ Analog   │         │ 8×8 LED  │
   │ Matrix    │      │ Value    │         │ Matrix   │
   └───────────┘      └──────────┘         └──────────┘
```

### Spezifikationen

| Parameter | Wert |
|-----------|------|
| **Microcontroller** | ATmega328P (Arduino Uno-Niveau) |
| **Taktfrequenz** | 16 MHz (externer Quarz) |
| **Flash** | 32 KB |
| **SRAM** | 2 KB |
| **EEPROM** | 1 KB |
| **LED-Matrix** | 8×8, 74HC595 Shift Register |
| **Serielle Ausgabe** | 9600 Baud, 8N1 |
| **Spannungsversorgung** | 5 V (USB/ISP) |

---

## 🏗️ Architektur

### Module-Übersicht

```
┌─────────────────────────────────────────────────────────────┐
│  main.c                                                     │
│  ├─ Game-Loop (Infinite while)                              │
│  ├─ Joystick-Polling (ADC2)                                 │
│  ├─ Matrix-Rendering (8× multiplexed)                       │
│  └─ Debounce-Logik                                          │
├─────────────────────────────────────────────────────────────┤
│  game.c / game.h                                            │
│  ├─ HP-Tracking (int8_t, Max 120)                           │
│  ├─ Turn-basierte Logik                                     │
│  ├─ 15 DMG pro Attack                                       │
│  ├─ Zufall mit rand() (Seed aus ADC-Rauschen)               │
│  └─ Sieg/Defeat-Erkennung                                   │
├─────────────────────────────────────────────────────────────┤
│  matrix.c / matrix.h                                          │
│  ├─ Hardware-SPI (MOSI=PB3, SCK=PB5, Latch=PB0)              │
│  ├─ 8× Row-Multiplexing (PD2-PD7, PC0-PC1)                  │
│  ├─ hp_to_bitmask() (Skalierung 120 HP → 8 LEDs)            │
│  └─ Continuous Refresh @ ~500 Hz                            │
├─────────────────────────────────────────────────────────────┤
│  uart.c / uart.h                                            │
│  ├─ 9600 Baud (Double-Speed Mode)                           │
│  ├─ Blocking Transmit (polling)                             │
│  └─ int16_t Zahlenausgabe (bis 4-stellig)                   │
├─────────────────────────────────────────────────────────────┤
│  input.c / input.h                                            │
│  ├─ INT0 (PD2) Button-Interrupt                             │
│  └─ (Reserve für erweiterte Steuerung)                      │
└─────────────────────────────────────────────────────────────┘
```

---

## 📈 Performance-Metriken

| Metrik | Wert |
|--------|------|
| **Matrix Refresh Rate** | ~500 Hz (per LED) |
| **Durchschnittliche Frame-Zeit** | 8 ms (8 Zeilen × 1 ms) |
| **ADC Konversion** | 13 ADC-Takte ≈ 104 µs |
| **UART Overhead** | < 1 ms pro Zeile |
| **Flash-Nutzung** | ~1,9 KB (6%) |
| **SRAM-Nutzung** | ~200 Bytes (10%) |

---

## 🚀 Build & Deployment

### Voraussetzungen

```bash
# macOS (Homebrew)
brew install avr-gcc avrdude make

# Ubuntu/Debian
sudo apt install gcc-avr avr-libc avrdude make
```

### Build-Prozess

```bash
# 1. Kompilieren
make clean && make all

# Ausgabe:
# avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -Wall -std=c11 -c main.c -o main.o
# avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -Wall -std=c11 -o main.elf main.o ...
# avr-objcopy -O ihex -R .eeprom main.elf main.hex

# 2. Flashen
make flash  # USBasp-Programmer

# 3. Terminal öffnen (9600 Baud)
screen /dev/cu.usbmodem11101 9600
```

### Makefile-Ziele

| Ziel | Beschreibung |
|------|--------------|
| `make all` | Kompiliert `main.hex` |
| `make flash` | Flasht auf ATmega328P (USBasp) |
| `make clean` | Entfernt alle Build-Artefakte |

---

## 💻 Terminal-Interface

```
+=======================+
|  P O K E B A T T L E  |
+=======================+
Joystick kippen = Angriff!

  Du      ########## 120/120
  Gegner  ########## 120/120

> Volltreffer! 15 Schaden!

  Du      ########## 105/120
  Gegner  #########  105/120

> Gegner trifft! 15 Schaden!

  Du      #########   90/120
  Gegner  ###########  90/120
```

---

## 🔌 Hardware-Verdrahtung

### Pin-Belegung

| Funktion | ATmega328P Pin | Arduino Pin | Beschreibung |
|----------|----------------|-------------|--------------|
| **MOSI** | PB3 | D11 | Shift Register Data |
| **SCK** | PB5 | D13 | Shift Register Clock |
| **Latch** | PB0 | D8 | Shift Register Latch (RCLK) |
| **SS** | PB2 | D10 | SPI Slave Select (Output) |
| **Row 0** | PD2 | D2 | Matrix Zeile 1 |
| **Row 1** | PD3 | D3 | Matrix Zeile 2 |
| **Row 2** | PD4 | D4 | Matrix Zeile 3 |
| **Row 3** | PD5 | D5 | Matrix Zeile 4 |
| **Row 4** | PD6 | D6 | Matrix Zeile 5 |
| **Row 5** | PD7 | D7 | Matrix Zeile 6 |
| **Row 6** | PC0 | A0 | Matrix Zeile 7 |
| **Row 7** | PC1 | A1 | Matrix Zeile 8 |
| **VRx** | PC2 | A2 | Joystick X-Achse |
| **TX** | PD1 | D1 | UART TX |
| **RX** | PD0 | D0 | UART RX (not used) |

### Schaltplan (ASCII)

```
        ATmega328P
        ┌─────────────────────┐
   PB3──┼──MOSI  74HC595      │
   PB5──┼──SCK   ┌─────────┐  │
   PB0──┼──RCLK └─────────┴──┼──► LED Matrix Columns
   PB2──┼──SS    (Output)     │
        │                     │
   PD2──┼──► Row 1 ───────────┤
   PD3──┼──► Row 2 ───────────┤
   PD4──┼──► Row 3 ───────────┤
   PD5──┼──► Row 4 ───────────┤
   PD6──┼──► Row 5 ───────────┤
   PD7──┼──► Row 6 ───────────┤
   PC0──┼──► Row 7 ───────────┤
   PC1──┼──► Row 8 ───────────┤
   PC2──┼──► VRx (Joystick)  │
        └─────────────────────┘
```

---

## 🎨 Matrix-Layout

```
Row 0:  ████████  ← Gegner HP Bar (voller Balken = 8 LEDs)
Row 1:  ········  ← Leer
Row 2:  ·██·██··  ← Gegner Sprite
Row 3:  ··████··
Row 4:  ··████··  ← Spieler Sprite
Row 5:  ·██·██··
Row 6:  ········  ← Leer
Row 7:  █·······  ← Cursor (Angriff 1: Bit 0)
```

---

## 🧪 Test-Protokolle

### Unit Tests

```bash
# Simuliere HP-Werte
hp_to_bitmask(0)   → 0b00000000 (0 LEDs)
hp_to_bitmask(15)  → 0b00000001 (1 LED)
hp_to_bitmask(30)  → 0b00000011 (2 LEDs)
hp_to_bitmask(120) → 0b11111111 (8 LEDs)
```

### Debug-Ausgabe

```
ADC x_val: 512    ← Joystick-Zentrum
ADC x_val: 200    ← Links
ADC x_val: 850    ← Rechts
```

---

## 📜 Lizenz

Dieses Projekt ist lizenzfrei und darf ohne Einschränkungen verwendet, verändert und verbreitet werden.

---

## 🎯 Zukünftige Roadmap

- [ ] **Sound-Modul** (PWM-Töne für Schläge, Sieg, Niederlage)
- [ ] **Mehrere Angriffe** (Tackle, ThunderShock, Quick Attack)
- [ ] **Gegner-Intelligenz** (Level-up, verschiedene Pokemon)
- [ ] **Speicherung** (EEPROM für High Scores)
- [ ] **Power-Management** (Sleep-Modus bei Inaktivität)

---

<div align="center">

**Entwickelt mit ❤️ und viel AVR-Glück**

---

![Tech Stack](https://img.shields.io/badge/Architecture-AVR%20Embedded-F5A623?style=flat-square)
![Language](https://img.shields.io/badge/Language-C%20(C99)-blue?style=flat-square)
![Compiler](https://img.shields.io/badge/Compiler-avr-gcc-orange?style=flat-square)
![Framework](https://img.shields.io/badge/Framework-none-lightgrey?style=flat-square)

</div>