# Galculus

Galculus is an experimental C++ framework for building distributed embedded AI agents.

The long-term goal is to provide a runtime where embedded devices, RTOS systems, edge computers, and LLM nodes can communicate through a common agent/message system.

## Current Status

Core v0.1 is implemented.

It includes:

- Agent lifecycle
- Event and Message types
- Runtime dispatch
- Result/Error handling
- In-memory message transport
- Binary protocol encoding
- Frame encoding
- Byte transport abstraction
- In-memory byte transport
- TCP byte transport
- Message-to-byte adapter
- Framed message transport
- Unit tests
- Local hello-agent example
- Two-node TCP example

## Build

From the repo root:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure