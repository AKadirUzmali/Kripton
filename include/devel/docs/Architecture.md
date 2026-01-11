# Architecture
```mermaid
    Config --> Core
    Core --> Log
    Core --> Trace
    Core --> Test

    Log --> Instrumentation
    Trace --> Instrumentation
    Test --> Verification
```