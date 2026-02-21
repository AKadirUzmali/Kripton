# Standardization (Standardizasyon)

---

## Amaç ve Kapsam

Bu dökümanın amacı:

- Proje mimarisinde kullanmak için bir **standardizasyon** belirlenmesi
- Bu standardizasyon **doğrultusunda** projenin geliştirilmesi
- Geleceğe yönelik **uyumluluk** problemlerini önceden tahmin ve tespit etmek
- **Gelişime açık** bir standart sunmak

---

## Değişken İsimlendirmeleri

- Sınıf üyesi değişkenin başında **m_**
- Sınıf üyesi sabit **static** ise **s_**
- Fonksiyon argümanı anlaşılması için başında **ar_**
- Normal değişkenler için **vv_**
- Kod içinde oluşturulan sabit değişkenler için **ss_**
- Geçici değişkenler için **tm_**
- Küresel kullanım değişkenler için **gg_**

---

## Kodlama Standartları

- C++17 minimum
- `-Wall -Wextra -Werror`
- Undefined Behavior **kabul edilemez**

Dosya isimlendirme:

- `snake_case.hpp`
- `snake_case.cpp`