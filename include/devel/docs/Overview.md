# Developer

Developer, geliştirici modunda çalışan bir **trace / log / test**
altyapısıdır.

## Bileşenler

- Trace System (`TRACE_SCOPE`, `TRACE_FUNC`)
- Logging System (`LOG`, `LOG_MSG`, kategori bazlı log)
- Test & Expect System (`EXPECT_MSG`, `ASSERT_MSG`)

## Akış Diyagramı

```mermaid
    main->>isEven: call
    isEven->>TRACE_SCOPE: start
    isEven->>LOG: message
    isEven->>EXPECT_MSG: validate
    isEven-->>main: bool
```