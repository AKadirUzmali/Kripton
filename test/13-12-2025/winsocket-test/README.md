# Test Adı: Winsock API

## Test Açıklaması
Bu test, şu an için **Windows işletim sistemi** üzerinde **Winsock testi** yapılmış olup,
gelecekte diğer işletim sistemlerine uyumluluk sağlanması planlanmaktadır.

Projede, manuel derleme işlemiyle **.cpp** dosyaları ve **.exe** dosyaları oluşturulmuştur.
Winsock yapısının geliştirilme aşamasında karşılaşılan bazı sorunlar nedeniyle, bu testler manuel olarak yapılmıştır.
Amaç sadece basit bir sunucu ile istemci arasında mesaj gönderimi ve bu mesajın istemci tarafından alınmasını sağlamak.

## Özellikler
- **Winsock Testi**: Windows işletim sistemi için Winsock API'sı üzerinde bağlantı testi yapılmıştır.
- **Manuel Derleme**: Proje, manuel derleme yöntemiyle oluşturulmuştur. **.cpp** dosyaları ve çalıştırılabilir **.exe** dosyaları mevcuttur.

## Gereksinimler
- **Windows 10** veya daha yeni bir sürüm (Winsock testi için).
- C++ derleyicisi (**g++**)