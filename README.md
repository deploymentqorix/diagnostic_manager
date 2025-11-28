# daignostic_manager

> A modular Diagnostic Manager for vehicle/embedded systems that handles diagnostic events, DTC lifecycle, condition monitoring, and communication with diagnostic services (UDS / SOVD style workflows).

---

## 📌 Overview

daignostic_manager is designed to provide a central, scalable, and thread-safe mechanism for managing vehicle diagnostic data. It coordinates diagnostic trouble codes (DTCs), event memory, conditions, and interfaces with diagnostic communication layers to support monitoring, reporting, and clearing of faults.

This project is suitable for AUTOSAR-based or AUTOSAR-inspired platforms and modern SDV (Software Defined Vehicle) architectures.

---

## ✨ Key Features

* ✅ Centralized Diagnostic Trouble Code (DTC) management
* ✅ Event memory with circular buffer support
* ✅ Condition-based diagnostic triggering
* ✅ Thread-safe architecture
* ✅ Configurable overflow and retention policies
* ✅ Diagnostic session and request handling support
* ✅ Compatible with UDS / ISO 14229 diagnostic concepts
* ✅ Extensible API structure

---

## 📂 Project Structure

```
daignostic_manager/
│
├── include/
│   ├── core/
│   │   ├── DtcManager.hpp
│   │   ├── EventMemory.hpp
│   │   ├── Condition.hpp
│   │   └── DiagnosticInterface.hpp
│   │
│   └── utils/
│       └── Logger.hpp
│
├── src/
│   ├── DtcManager.cpp
│   ├── EventMemory.cpp
│   └── Condition.cpp
│
├── tests/
│   └── unit/
│
├── CMakeLists.txt
└── README.md
```

---

## ⚙️ Core Components

### 1. DtcManager

Handles DTC creation, status updates, storage, and clearing logic. Ensures compliance with diagnostic life-cycle rules.

### 2. EventMemory

Thread-safe circular buffer storing diagnostic events with FIFO / overflow handling policies.

### 3. Condition

Defines logical diagnostic conditions for triggering events based on system state.

### 4. Diagnostic Interface

Provides communication hooks for diagnostic servers such as DCM or remote diagnostic layers.

---

## 🔧 Build Instructions

### Prerequisites

* CMake 3.10+
* GCC / Clang supporting C++17

### Build Steps

```bash
git clone <repository-url>
cd daignostic_manager
mkdir build && cd build
cmake ..
make
```

---

## 🚀 Usage Example

```cpp
#include "DtcManager.hpp"

int main() {
    core::DtcManager manager;
    manager.registerDtc(0x1234);
    manager.setDtcStatus(0x1234, true);
    return 0;
}
```

---

## 🧪 Testing

Run unit tests with:

```bash
ctest --verbose
```

---

## 📖 Configuration

Configuration can be managed via JSON / YAML files or compile-time parameters depending on the deployment needs.

Example parameters:

* Max DTC Count
* Event Memory Capacity
* Overflow Policy
* Diagnostic Session Type

---

## 🔐 Thread Safety

All core modules are designed to be thread-safe using mutex locking and atomic operations where required.

---

## 🛠 Future Enhancements

* Web-based Diagnostic Dashboard
* Cloud telemetry integration
* AI-based predictive diagnostics
* Remote OTA diagnostic updates

---

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Open a pull request

---

## 📄 License

This project is licensed under the MIT License. See the LICENSE file for details.

---

## 📬 Contact

For suggestions or issues:

* Author: Nikhil
* Email: [nikhilydav1791@gmail.com]
* Project: daignostic_manager

---

> Built for reliability, scalability, and modern automotive diagnostic systems 🚗⚙️
