# daignostic_manager

> Runtime Diagnostic Manager implementation providing AUTOSAR-like diagnostic services, condition monitoring, event handling, and DTC management with a layered architecture.

---

## 📌 Overview

The **daignostic_manager** project implements a structured runtime diagnostic framework inspired by AUTOSAR Adaptive diagnostics. It includes core diagnostic entities such as Conditions, Events, Operation Cycles, Monitors, and DTC Information, along with shared ara::core utilities like `InstanceSpecifier` and `Result/Future`.

This layout clearly separates:

* AUTOSAR diagnostic API layer (`ara-diag`)
* Diagnostic manager implementation (`diagnostic-manager`)
* Build configurations and runtime structure

---

## 📂 Actual Project Structure

```
DIAGNOSTIC-MANAGER-RUNTIME/
│
├── .history/
├── .vscode/
│
├── ara-diag/
│   ├── buildconfig/
│   └── dev/
│       ├── inc/
│       │   ├── private/
│       │   └── public/ara/
│       │       ├── core/
│       │       │   ├── instance_specifier.h
│       │       │   └── result_future.h
│       │       └── diag/
│       │           ├── condition.h
│       │           ├── dtc_information.h
│       │           ├── event_types.h
│       │           ├── event.h
│       │           ├── monitor_types.h
│       │           ├── monitor.h
│       │           └── operation_cycle.h
│       └── src/
│           ├── condition.cpp
│           ├── dtc_information.cpp
│           ├── event.cpp
│           ├── monitor.cpp
│           └── operation_cycle.cpp
│
├── diagnostic-manager/
│   ├── buildconfig/
│   └── dev/
│       ├── inc/
│       │   ├── ara/core/
│       │   │   ├── instance_specifier.h
│       │   │   └── result_future.h
│       │   ├── common/
│       │   ├── dtc/
│       │   │   └── dm_dtc.h
│       │   ├── event/
│       │   │   └── dm_event.h
│       │   ├── lib/
│       │   └── operationcycle/
│       └── src/
│           ├── common/
│           ├── dtc/
│           │   └── dm_dtc.cpp
│           ├── event/
│           │   └── dm_event.cpp
│           ├── lib/
│           ├── operationcycle/
│           │   └── dm_operation_cycle.cpp
│           └── main.cpp
│
├── functional_testing/
│   └── Test_app/
│       ├── buildconfig/
│       ├── inc/
│       │   └── app.h
│       └── src/
│           ├── app.cpp
│           ├── test.cpp
│           └── test_diag.cpp
│
└── README.md
```

---

## 🧩 Module Breakdown

### ara-diag Layer (AUTOSAR Diagnostic API)

This section defines the standardized diagnostic interfaces.

* **core/**

  * `instance_specifier.h` – Identifies diagnostic elements uniquely
  * `result_future.h` – Result handling with Future/Promise semantics

* **diag/**

  * `condition.h` – Defines diagnostic conditions
  * `dtc_information.h` – DTC metadata and attributes
  * `event.h` & `event_types.h` – Diagnostic event definitions
  * `monitor.h` & `monitor_types.h` – Diagnostic monitor interfaces
  * `operation_cycle.h` – Operation cycle handling

Source files implement their runtime behavior under `src/`.

---

### diagnostic-manager Layer (Implementation Layer)

This layer provides project-specific logic wrapping AUTOSAR-like APIs.

* **dtc/**

  * `dm_dtc.h` – Core DTC management logic

* **event/**

  * `dm_event.h` – Event management and propagation

* **operationcycle/**

  * Runtime handling of operation cycle change logic

* **common/**

  * Shared utility definitions

---

## ⚙️ Core Functionalities

* Diagnostic Condition evaluation
* Event reporting and lifecycle handling
* DTC registration, update and clearing
* Monitor execution and state tracking
* Operation cycle management
* AUTOSAR-style Result/Future based APIs

---

## 🔧 Build Instructions

```bash
cd DIAGNOSTIC-MANAGER-RUNTIME
cd buildconfg
conan create .
```

---

## 🚀 Example Usage

```cpp
ara::diag::Event myEvent("EngineOverheat");
myEvent.ReportFailed();
```

```cpp
ara::diag::OperationCycle cycle("IgnitionCycle");
cycle.StartCycle();
```

---

## ✅ Design Goals

* AUTOSAR Adaptive compatibility
* High modularity
* Reusability across SDV platforms
* Clear separation of interface & implementation
* Maintainable diagnostics runtime

---

## 🛠 Planned Enhancements

* Integration with remote diagnostics service
* Persistent fault storage
* Diagnostic CLI tool
* REST API interface for monitoring

---

## 📄 License

MIT License

---

## 👤 Author

Nikhil Yadav
Project: daignostic_manager
Type: Diagnostic Runtime Framework for SDV systems

---

If you want this README exported as a file or enhanced with diagrams / badges, just tell me.
