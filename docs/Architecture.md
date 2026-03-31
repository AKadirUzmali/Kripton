# Mimari (Architecture)
## İçindekiler

- [Standardizasyon](Standardization.md)
- [Güvenli Politika](Policy.md)
- [Sistem Güvenliği](Security.md)
- [Geliştirici](Developer.md)
- [Derleme](Build.md)
- [Kayıtlar](Logs.md)

---

## Amaç ve Kapsam

Bu dokümanın amacı:

- Proje mimarisini **tek bir referans noktası** altında toplamak
- Yeni geliştiricilerin projeye **hızlı adapte olmasını** sağlamak
- Mimari kararların **nedenlerini** kayıt altına almak
- Uzun vadede **bakım ve evrimi** kolaylaştırmak

Kapsam dışı:

- Düşük seviyeli algoritma detayları
- API kullanım örnekleri (ayrı dokümantasyonlarda ele alınır)

---

## Genel Bakış

Proje, **katmanlı ve modüler** bir mimari yaklaşım benimser. Temel hedefler:

- Platform bağımsızlık
- Güvenli ve öngörülebilir davranış
- Test edilebilirlik
- Uzun ömürlü bakım

**Not:** Mimari, statik değil; ihtiyaçlara göre evrilebilir şekilde tasarlanmıştır.

---

## Tasarım Prensipleri

- **Single Responsibility Principle (SRP)**: Her modül tek bir sorumluluğa sahiptir
- **Loose Coupling**: Modüller arası bağımlılık minimumda tutulur
- **High Cohesion**: İlgili işlevler aynı modül altında toplanır
- **Fail Fast**: Hatalar erken yakalanır

---

## Mimari

```text
+-------------------+
|     Developer     |-------
+-------------------+      |
          |                |
          |                |
          |                |
+-------------------+      |        +------------------------+
|       Core        |------|------> |   Executable Program   |
+-------------------+      |        +------------------------+
          |                |
          |                |
          |                |
+-------------------+      |
|       Tools       |-------
+-------------------+
```

Ana katmanlar üç parçadan oluşmaktadır:
- Developer (Geliştiric)
- Core (Çekirdek)
- Tools (Araçlar)

Bu üç ana katmandan geliştirici çekirdek yapının testleri için tasarlanmıştır
ve araçlar ise çekirdekte yapılan işlemlerin kolaylığını sağlamak için yapılmıştır.
Bunun dışında kalanlar ise çekirdek parçanın içinde oluşturulanlardır.

---

## Performans ve Kaynak Yönetimi

- Dinamik bellek kullanımı minimize edilir
- Kopyalama yerine **move semantics**
- Hot-path fonksiyonlar ölçümlenir

```cpp
[[nodiscard]] inline bool is_valid() const noexcept;
```

## Yardımcı Fonksiyonlar ve İşlevler

- İstenildiği durumda her yapının ya da belirli yapıların kullanabilmesinin sağlanması
- Kullanılmadığı durumda derleyici hatası olmaması adına ön tanımlama
- Araç setine entegre edilerek kolay erişim sağlama

```cpp
[[maybe_unused]] std::string to_utf8(const std::u32string&) noexcept;
```

---

## Platform ve Derleme

Desteklenen platformlar:

- Windows (G++)
- Linux (G++)
- FreeBSD (G++)

---

## Riskler ve Teknik Borç

| Risk | Etki | Önlem |
|----|----|----|
| API şişmesi | Orta | İnce arayüzler |
| Platform farkları | Yüksek | CI testleri |
| Kripto bağımlılığı | Yüksek | Soyutlama |