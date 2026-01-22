# Derleme (Build)

Bu dökümanda ise projenin **derleme** işlemleri hakkında bilgilendirme yapılacaktır.

---

## Amaç ve Kapsam

Bu dokümanın amacı:

- Projenin **derlenme** aşamasını anlatmak
- **Çoklu işletim sistemi** ile derleyebilmeyi sağlamak
- Derleme sırasında belirlenen **ön talimatları** amacı ile anlatmak
- Programın son aşaması olan derlemenin **önemi** ve nedenini anlatmak

---

## Derleme Öncesi ve Derleme Aşaması

```text
+-------------------+
|       Design      |
+-------------------+
          |
          v
+-------------------+
|      Coding       |
+-------------------+
          |
          v
+-------------------+
|      Testing      |
+-------------------+
          |
          v
+-------------------+
|      Building     |
+-------------------+
```

Katmanlar yukarıdan aşağıya bağımlıdır. Ters yönde bağımlılık **yasaktır**.

---

## Derleme Modeli

- Varsayılan olarak **G++** derleyicisi seçilmiştir
- Seçilmiş işletim sistemleri için uyumluluğu test edilir
- İşletim sistemine uygun API yapısını kullanarak derlemeyi mümkün kılmayı amaçlamak

| Platform | İşletim Sistemi | Derleyici |
|------|-----|----|
| Unix | Bsd | G++ |
| Unix-Like | Linux | G++ |
| WindowsNT | Windows | G++ |