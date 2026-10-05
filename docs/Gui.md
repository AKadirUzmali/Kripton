# Grafik Arayüzü (GUI)
## Amaç ve Kapsam

Bu dokümanın amacı:

- Projenin **grafik arayüzü geliştirme** aşamasını anlatmak
- Geliştirilen arayüzün geliştirici kontrolü ile **test** edilmesini sağlamak
- Sonuçları **kayıt** altına alıp hataları kontrol edebilmek
- Programın **performans** değerlerini ölçmek ve daha iyi optimizasyon yapmak
- Ön teşhis ile gelecekti **problemleri ortadan kaldırmak**
- Kullanılabilir ve **çapraz platformda** çalışabilen performanslı bir **gui** tasarlamak

---

## GUI Standartı

Gui standartı olarak wxWidgets seçildi. Performanslı, Çapraz Platform Destekli ve Ücretsiz olması
sebebi ile seçildi. Windows, Linux ve FreeBsd işletim sistemlerinde aktif olarak kullanılan
grafik altyapısına göre derler bu sayede performans kazancı sağlar fakat işletim sisteminden
işletim sistemine görüntü farkı oluşabilmektedir.

- wxWidgets gui rakibi olan Qt'ye nazaran daha kolay olduğu ve ücretsiz olduğu
için seçildi

- Amaç çok gelişmiş ve muazzam detalı, güzel bir grafik arayüzü geliştirmek değil.
Performanslı ve kullanışlı olması yeterli olan bir gui tasarımı yapmak amaç.

- İşletim sistemlerinin hepsinde aynı görüntü de olmadığı için olabildiğince
sabit bir görüntü ile oluşturulmaktadır.

- Testler yapılırken gelecekte daha rahat okunabilir olması adına
yapılan standarttır. Bu standart sayesinde okunabilirik eski ve hatalı standarta göre artmıştır.

- Gelecekte kayıt sistemi olarakta kullanılabilme imkanı sunmaktadır. Sadece fikir aşamasındadır.
Kayıt sistemi olarak kullanılması durumunda yapılanları ekrana çıktı vermesi istenecektir fakat
sonsuz döngü hatalarının önlenmesi lazım aksi takdirde bellek tüketimi aşırı fazla olacaktır.

---

## Geliştirici Öncesi ve Geliştirici Aşaması

```text
+-------------------+
|      Windows      | -------
+-------------------+       |
          |                 |
          v                 |
+-------------------+       |    +-------------------+
|      FreeBsd      | ---------> |     wxWidgets     |
+-------------------+       |    +-------------------+
          |                 |
          v                 |
+-------------------+       |
|       Linux       | -------
+-------------------+
```