# Geliştirici (Developer)
## Amaç ve Kapsam

Bu dokümanın amacı:

- Projenin **geliştirici** aşamasını anlatmak
- Geliştirilen yapıların geliştirici kontrolü ile **test** edilmesini sağlamak
- Sonuçları **kayıt** altına alıp hataları kontrol edebilmek
- Programın **performans** değerlerini ölçmek ve daha iyi optimizasyon yapmak
- Ön teşhis ile gelecekti **problemleri ortadan kaldırmak**

---

## Test Standartı

Testler yapılırken gelecekte daha rahat okunabilir olması adına
yapılan standarttır. Bu standart sayesinde okunabilirik eski ve hatalı standarta göre artmıştır.

- Test tarihi **YIL/AY/GÜN** olarak ayarlanmıştır çünkü karakter büyüklüğü sırasında göre dosyalar listelenmekte
    -> 01/01/2026 daha yeni olmasına rağmen 02/10/2025 daha yeni gibi gözükebilir, bu hataları önlemek için yapıldı

- Test sonunda **logs** klasörü oluşturulup, işletim sisteminin adını da içerecek şekilde test sonuçlarını içeren kayıt dosyası olacak
    -> logs/20260101-blabla-linux.log | logs/20260101-blabla-windows.log | logs/20260101-blabla-bsd.log

- Test yaparken oluşturulan derleme dosyaları **.exe .o** vs. **.gitignore** ile göz ardı edilecek, test etmek isteyen kişi belirnen
  derleme yöntemi ile derleme yapabilecek. Derleme yöntemi ana test dosyasında paylaşılacaktır.

- Test işlemleri son derleme aşamasında program yükü olmayacak şekilde tasarlanıp kullanılacaktır.
- Test sırasında yapılanlar daha rahat okunması için hem dosya hem de konsol ekranına çıktı verecek şekilde olacaktır.
- Test dosyalarının kayıtı için belirlenen klasör isimi **logs** dur. Bu klasör olmaması durumunda dosyayı kayıt yapmayabilir!

Bu yeni getirilen basit standartlar ile hem daha okunabilir hem de daha düzgün bir test standartı oluşturulmuş olacaktır.
Eski ve eksik sistemde bunlardan bazıları olsada belirlenen bir standart olmadığı için sürekli olarak farklı farklı
tarzda test işlemleri yapılmış olabiliyor.

Yeni test standartı 2026/01/25 tarihinde saat 15:00 da Abdulkadir U. tarafından tasarlanmıştır.

---

## Geliştirici Öncesi ve Geliştirici Aşaması

```text
+-------------------+
|       Coding      |
+-------------------+
          |
          v
+-------------------+
|   Developer Test  | <------
+-------------------+       |
          |                 |
          v                 |
+-------------------+       |
|  Fixing/Patching  | -------
+-------------------+
          |
          v
+-------------------+
|       Build       |
+-------------------+
```