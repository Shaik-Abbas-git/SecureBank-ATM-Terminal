# SecureBank ATM Terminal 🏦
<p align="center">
  <img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white" />
  <img src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black" />
  <img src="https://img.shields.io/badge/ARM-0091BD?style=for-the-badge&logo=arm&logoColor=white" />
  <img src="https://img.shields.io/badge/GCC-A3D900?style=for-the-badge&logo=gnu&logoColor=black" />
</p>

An industrial-grade embedded ATM terminal simulation featuring a bare-metal Finite State Machine on the NXP LPC2148 (ARM7TDMI-S) and a Linux-based C daemon for secure, real-time transaction processing. 

This project demonstrates hardware-software co-design, asynchronous UART communication, peripheral interfacing, and POSIX-compliant backend server management.

---

## Table of Contents
- [Project Overview](#project-overview)
- [System Workflow & Block Diagram](#system-workflow--block-diagram)
- [Key Features](#key-features)
- [Tech Stack & Hardware](#tech-stack--hardware)
- [Pin Configuration](#pin-configuration)
- [System Architecture & Protocol](#system-architecture--protocol)
- [Repository Structure](#repository-structure)
- [Build & Execution Instructions](#build--execution-instructions)
- [Output Gallery](#output-gallery)
- [Future Enhancements](#future-enhancements)

---

## Project Overview
SecureBank ATM Terminal bridges a physical bare-metal microcontroller environment with a backend Linux server. Users authenticate via an EM18 RFID scanner and a custom 4x4 matrix keypad. The LPC2148 manages the local FSM (Finite State Machine) and UI rendering on a 16x2 LCD, while asynchronously dispatching financial payload requests to a Linux daemon over UART. The server manages a flat-file database, validating PINs, calculating balances, and logging transactions.

---

## Block Diagram & System Workflow:

**1. Block Diagram:**
<img width="1672" height="941" alt="block-diagram (2)" src="https://github.com/user-attachments/assets/d42935b5-c664-4a4f-9f59-7b1986dcdfe2" />

**2. System Workflow:**
<img width="1536" height="1024" alt="flowchart" src="https://github.com/user-attachments/assets/aee8f0dc-63b8-411f-aa21-d9bf4863fa6e" />


---

## Key Features
* **Two-Factor Authentication Simulation:** Requires both a valid RFID tag read and a secure 4-digit PIN.
* **Core Banking Transactions:** Supports Deposits, Withdrawals (in multiples of 100), and Balance Inquiries.
* **Mini-Statement Generation:** Retrieves and displays the three most recent transactions dynamically on the LCD.
* **Secure PIN Management:** Allows authenticated users to change their PIN directly from the terminal.
* **Event-Driven UI:** Custom multi-page menu navigation utilizing a matrix keypad.
* **Robust Backend Daemon:** Linux C server featuring POSIX signal handling (`SIGINT`) for graceful shutdown and data persistence.

---

## Tech Stack & Hardware

### Software & Tools
* **Firmware:** Embedded C, Keil µVision, Flash Magic
* **Backend Server:** C (POSIX), GCC, Linux (Ubuntu/WSL2)
* **Protocols:** UART (9600 Baud, 8N1, Raw Mode)

### Hardware Components
* NXP LPC2148 Microcontroller (ARM7TDMI-S Architecture)
* 16x2 Character LCD Display (Configured in 8-bit mode)
* 4x4 Membrane Keypad
* EM18 RFID Reader Module (125 kHz)
* USB-to-TTL CP2102/PL2303 Serial Adapter

---

## Pin Configuration

| Component | LPC2148 Port/Pin | Description |
| :--- | :--- | :--- |
| **LCD Data** | `P0.16 - P0.23` | 8-bit Data Lines (`D0-D7`) |
| **LCD Control**| `P0.5`, `P0.6`, `P0.4` | Register Select (`RS`), Read/Write (`RW`), Enable (`EN`) |
| **Keypad Rows**| `P1.16 - P1.19` | GPIO Outputs (Scanned low sequentially) |
| **Keypad Cols**| `P1.20 - P1.23` | GPIO Inputs (Pulled high, reads low on press) |
| **EM18 RFID** | `P0.9` | UART1 `RXD` (Interrupt-driven via VIC) |
| **PC Server** | `P0.0`, `P0.1` | UART0 `TXD` / `RXD` (Polling mode) |

---

## System Architecture & Protocol

### Firmware State Machine
The LPC2148 firmware operates on a strict FSM:
1. `STATE_IDLE`: Waits for a UART1 interrupt triggered by the EM18 RFID reader.
2. `STATE_WAIT_PIN`: Captures keypad input, masks it to the LCD, and requests server validation.
3. `STATE_MENU`: Renders a 2-page UI, captures transaction requests, and routes responses.

### UART Communication Protocol
The MCU and PC communicate via a custom request/reply string protocol, terminated by `$`.
* **MCU Request Format:** `#TXN:WITHDRAW#AMOUNT:500$`
* **PC Reply Format:** `@OK#BALANCE:4500.00$` or `@ERR#INSUFFICIENT_BALANCE$`

---

## Repository Structure

```text
SecureBank-ATM-Terminal/
├── firmware/                 # Bare-metal ARM7 codebase
│   ├── Startup.s             # Assembly startup file
│   ├── main.c                # Core FSM and application logic
│   ├── keypad.c / keypad.h   # Matrix keypad scanning driver
│   ├── lcd.c / lcd.h         # 16x2 LCD display driver
│   ├── rfid.c                # UART1 ISR for EM18 scanner
│   ├── uart0.c / uart0.h     # UART0 polling driver for PC link
│   ├── delay.c / delay.h     # Software delay loops
│   └── types.h / define.h    # Standard types and bitwise macros
│
└── server/                   # Linux backend daemon
    ├── pc_main.c             # Server FSM, logic, and signal handling
    ├── uart.c / uart.h       # POSIX termios serial port driver
    ├── users.txt             # Flat-file user database
    └── transactions.txt      # Append-only transaction ledger
```
## Build & Execution Instructions

### Phase 1: Linux Server Setup
1. Clone the repository and navigate to the server directory:
   ```bash
   git clone [https://github.com/Shaik-Abbas-git/SecureBank-ATM-Terminal.git](https://github.com/Shaik-Abbas-git/SecureBank-ATM-Terminal.git)
   cd SecureBank-ATM-Terminal/server
   ```
2. Compile the daemon using GCC:
   ```bash
   gcc pc_main.c uart.c -o atm_server
   ```
3. Connect your USB-to-TTL adapter to the Linux machine. Identify the port (e.g., `/dev/ttyUSB0`) and run the server:
   ```bash
   ./atm_server /dev/ttyUSB0
   ```
   *(Ensure `users.txt` is populated in the same directory before running).*

### Phase 2: Firmware Setup (Windows)
1. Open **Keil µVision** and create a new project targeting the **NXP LPC2148**.
2. Add all `.c`, `.h`, and `.s` files from the `/firmware` directory to your Source Group.
3. Configure target options for Hex output and compile the project (Press `F7`).
4. Connect the LPC2148 board to your PC.
5. Use **Flash Magic** to flash the generated `.hex` file into the MCU.
6. Press the hardware **Reset** button on the LPC2148 to boot the terminal.

---

## Output Gallery

<!-- TODO: Replace the placeholder links below with your actual hardware output images -->
**1. Authentication Phase:**
<img width="1672" height="941" alt="auth-screen" src="https://github.com/user-attachments/assets/2a821b09-7473-4553-8bd7-2b8ccf9373cc" />


**2. Main Menu Interface:**
<img width="1672" height="941" alt="Main_menu" src="https://github.com/user-attachments/assets/b77cf57f-5291-41c0-9d99-88df581517fd" />


**3. Transaction Success:**
<img width="1672" height="941" alt="Transaction" src="https://github.com/user-attachments/assets/65d339e5-70b5-427e-97a2-ed52de8be5b2" />


---

## Future Enhancements
* Transition from flat-file `.txt` storage to a lightweight SQL database (e.g., SQLite) on the Linux server.
* Implement AES encryption over the UART payload to prevent man-in-the-middle data sniffing.
* Add EEPROM integration on the LPC2148 for local caching of recent transactions during network drops.
