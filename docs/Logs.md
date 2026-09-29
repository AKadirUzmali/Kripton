# Kayıtlar (Logs)
## Amaç ve Kapsam

Bu dokümanın amacı:

- Projede yapılanların **kayıt** altına alınması
- Yapılanları **okumak** için basit bir kayıt metini
- Proje hakkında daha fazla **bilgi** sahibi olmak
- Yeniliklerin ve **değişiklikler** hakkında haberdar olmak

---

## v0.8.9

- Socket: Basit bir çıktı tarz düzeltmesi
- Server: Bazı **bellek, dead-lock, mantık hatalarının** giderilmesi ve
    çoklu istemci için ek düzenlemeler
- Test: **Multi-Chat** için sunucu aldığı veriyi diğer istemciler ile
    ona veriyi aktaran istemcinin adını kullanarak paylaşıyor ve
    bu sayede diğer istemcilerde mesajı gönderen istemci ve mesajından
    haberdar oluyor. Bazı hatalar, mantık hataları vs. olabilir
    fakat test genel amacına ulaşmıştır. İlerleyen zamanda **gui**
    ile ve daha yeni kod iyileştirmeleri ile bunlar sağlanabilir.
    **Log** sistemini ise sabit yapmak yerine dinamik olacak şekilde
    güncellenebilir ve kayıt sistemi daha okunaklı ve geleceğe dönük
    olmuş olur. Sadece Linux (Debian/GNU 13) üzerinde test edildi.

---

## v0.8.8

- Crash: **is_signal** fonksiyonu **has_signal** olarak yeniden isimlendirildi
- Thread Pool:
    + **Thread ID** bilgisine göre işlem sonlandırılmasını sağlamak
    + **Crash Handler** sınıfında oluşan **has_signal** sonrası işlemlerin sonlandırılamaması mantık hatası giderildi
- Logger: Kayıt işlemleri sırasında oluşan **race-condition** durumunu **mutex** ile önlemek fakat **performans** kayıplarına düşük ölçekte neden olabilir
- Policy: **Ban** sisteminde genel **ip** bazlı engelleme sistemi yerine **socket** bazlı engelleme sistemine geçildi
- Socket:
    + Birçok olan ya da olabilecek **mantık hataları** giderildi
    + **Recv** ve **Send** fonksiyonlarında oluşabilecek **bellek hataları** ya da **metin hataları** giderildi
    + **Recv** ve **Send** fonksiyonlarında veri alımı ve veri gönderimi işlemi optimize edildi
    + **Soket** kapatma ve sonlandırma sıralarındaki mantık hataları giderildi
    + **init** fonksiyonu ile sınıf **okunabilirliği** ve **kullanılabilirliği** arttırıldı
    + **Güvenlik** arttırıldı
    + Sürüm **Vch Hash** değeri güncellendi
- Server:
    + **Çoklu Sunucu** desteği eklendi
    + **Engelleme, Kayıt, Hata Kontrolleri, Durum Bayrağı vb.** sınıf özellikleri test edildi
    + **Performans** ve **Güvenlik** optimizasyonu yapıldı
    + Birçok **mantıksal** hata giderildi
- Client:
    + Birçok olan ya da olabilecek **mantık hataları** giderildi
    + **Recv** ve **Send** fonksiyonlarında oluşabilecek **bellek hataları** ya da **metin hataları** giderildi
    + **Recv** ve **Send** fonksiyonlarında veri alımı ve veri gönderimi işlemi optimize edildi
    + **Çoklu İstemci** ile tek bir sunucuya bağlanabilme desteği eklendi
    + **race-condition** problemleri çözümlendi
    + **Performans** ve **Güvenlik** optimizasyonu yapıldı
- Test:
    + **Server** ve **Client** sınıflarının testleri yapıldı
    + **Kayıt** dosyaları kontrol edildi ve hatalar giderildi
    + Testin **son kontrolleri** yapıldı
    + **BSD** ve **Windows** testleri yapıldı
    + **Linux (Debian/GNU)** ile tekrar test edildi