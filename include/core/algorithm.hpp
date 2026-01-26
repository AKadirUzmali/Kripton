// Abdulkadir U. - 2026/01/26
#pragma once

/**
 * Algorithm (Algoritma)
 * 
 * Şifreleme işlemleri için algoritma ihtiyacı olur.
 * Genel bir algoritma sınıfı oluşturarak şifreleme ve
 * şifre çözme işlemlerini yapmamızı kolaylaştıracak
 * ve standartlaştıracak
 * 
 * Not: Algorithm sınıfı çekirdek sınıf olmasına rağmen
 * dış ek araç ihtiyacı duyan tek sınıf olmaktadır.
 * Bunun sebebi ise karakter seti kaynaklı hataları önlemek içindir.
 */

// Include
#include <cstdint>
#include <string>

#include <tools/charset/utf.hpp>
using namespace tools::charset;

// Namespace
namespace core::algorithm
{
    // Class
    class Algorithm
    {
        public:
            static constexpr uint16_t MIN_LEN_NAME = 1;   // byte
            static constexpr uint16_t MAX_LEN_NAME = 64;  // byte

            static constexpr uint16_t MIN_LEN_KEY = 1;    // byte
            static constexpr uint16_t MAX_LEN_KEY = 4096; // byte

        private:
            std::string m_name;
            std::string m_key;

        private:
            inline void clear() noexcept;

            bool set_name(const std::string& ar_name) noexcept;
            bool set_key(const std::string& ar_key) noexcept;

        protected:
            virtual ~Algorithm();

        public:
            explicit Algorithm(
                const std::string& ar_name,
                const std::u32string& ar_key
            );

            virtual bool has_error() const noexcept;

            inline bool has_name() const noexcept;
            inline bool has_key() const noexcept;

            inline const std::string& get_name() const noexcept;
            inline const std::string& get_key() const noexcept;

            virtual bool encrypt(std::string& ar_text) noexcept = 0;
            virtual bool decrypt(std::string& ar_text) noexcept = 0;

            static inline bool is_valid_name(const std::string& ar_name) noexcept;
            static inline bool is_valid_key(const std::string& ar_key) noexcept;
    };

    /**
     * @brief Algorithm
     * 
     * Şifreleme için en gerekli olan şey
     * anahtardır. Kullanıcıdan hem anahtar verisin
     * alıyoruz hem de gelecekte basit karmaşıklıkları
     * önlemesi adına isim verisi alıyoruz.
     * 
     * Algoritma için bir isim ve anahtar belirlemeyi sadece
     * sınıf başlangıcında yapmasını istiyoruz fakat
     * buna ekstra bir güvenlik getirme ihtiyacı duymadık
     * çünkü gelecekte daha farklı şeyler istenebilir bu
     * yüzden kodun temelinden kaynaklı hata olmasını istemiyoruz.
     * Sadece basit düzeye gizlilik sağlıyoruz
     * 
     * @param string& Name
     * @param u32string Key
     */
    Algorithm::Algorithm(
        const std::string& ar_name,
        const std::u32string& ar_key
    )
    {
        if( !this->set_name(ar_name) || !this->set_key(utf::to_utf8(ar_key)) ) {
            this->clear();
            std::terminate();
        }
    }

    /**
     * @brief ~Algorithm
     * 
     * Sınıfta oluşturulan referansların bilgilerini temizleyecek
     * bu sayede şifreleme yapmasını engelleyerek gereksiz
     * sorunları ortadan kaldıracak fakat bu genel sınıfı baz alan
     * diğer sınıflar hatasız tasarlanmalı çünkü bu sınıf genel bir
     * tasarımdır
     */
    Algorithm::~Algorithm()
    {
        this->clear();
    }

    /**
     * @brief Clear
     * 
     * Sınıftaki isim ve anahtar bilgisini temizleyecek.
     * Daha çok yıkım işlemi için kullanılacak ve temizleme
     * işlemini yaptıktan sonra tekrar kullanılamaması adına
     * hatalı olmasını sağlayacak şekilde temizleme yapacak
     * 
     * Örnek: anahtar -> a3bazx1Qq && temizlik sonrası anahtar -> boş
     */
    void Algorithm::clear() noexcept
    {
        std::fill(this->m_name.begin(), this->m_name.end(), '\0');
        this->m_name.clear();
        
        std::fill(this->m_key.begin(), this->m_key.end(), U'\0');
        this->m_key.clear();
    }

    /**
     * @brief Has Error
     * 
     * Şifrleme işlemlerinde ilk önce hata olup olmadığını test etmeliyiz
     * çünkü kırık bir kilit kitlenemez ve kilidi açılamaz. Bu sebepten
     * temel sınıf olduğu için basitçe isim ve anahtar kontrolü yapacağız.
     * İsim kontrolü yapmasakta olur fakat gelecekte sorun yaratmaması adına
     * yapıyoruz. Sanal bir fonksiyon olduğu içinde, eğer istenirse üst
     * sınıflarda tekrardan tasarlanabilir.
     * 
     * @return bool
     */
    bool Algorithm::has_error() const noexcept
    {
        return !has_name() || !has_key();
    }

    /**
     * @brief Has Name
     * 
     * Şifreleme için geçerli bir isim verip vermemediğini
     * kontrol ederekten cevap döndürüyoruz.
     * 
     * @return bool
     */
    bool Algorithm::has_name() const noexcept
    {
        return is_valid_name(this->m_name);
    }

    /**
     * @brief Has Key
     * 
     * Şifreleme için geçerli bir anahtar verip vermemediğini
     * kontrol ederekten cevap döndürüyoruz.
     * 
     * @return bool
     */
    bool Algorithm::has_key() const noexcept
    {
        return is_valid_key(this->m_key);
    }

    /**
     * @brief Get Name
     * 
     * Algoritma için verilmiş olan isim verisini değiştirilemez
     * referans olarak döndürüyoruz çünkü isim değişikliği yapılması
     * karışıkların olmasını kaçınılmaz kılar.
     * 
     * @return const std::string&
     */
    const std::string& Algorithm::get_name() const noexcept
    {
        return this->m_name;
    }

    /**
     * @brief Get Key
     * 
     * Algoritma için verilmiş olan anahtar verisini değiştirilemez
     * referans olarak döndürüyoruz çünkü isim değişikliği yapılması
     * karışıkların olmasını kaçınılmaz kılar ve kritik seviyede
     * bir güvenlik problemi demektir çünkü bu sınıf amacı zaten
     * düzgünce şifreleme yapmak ve şifre çözmek
     * 
     * @return const std::string&
     */
    const std::string& Algorithm::get_key() const noexcept
    {
        return this->m_key;
    }

    /**
     * @brief Set Name
     * 
     * Algoritma için bir isim belirlenecek ve bunu
     * geçerli olup olmadığına göre kontrol edip belirleyecek
     * ya da hata direk hata döndürecek.
     * 
     * @param string& Name
     * @return bool
    */
    bool Algorithm::set_name(
        const std::string& ar_name
    ) noexcept
    {
        if( !is_valid_name(ar_name) )
            return false;
        
        this->m_name = ar_name;
        return true;
    }

    /**
     * @brief Set Key
     * 
     * Algoritma için bir anahtar belirlenecek ve bunu
     * geçerli olup olmadığına göre kontrol edip belirleyecek
     * ya da hata direk hata döndürecek.
     * 
     * @param string& Key
     * @return bool
    */
    bool Algorithm::set_key(
        const std::string& ar_key
    ) noexcept
    {
        if( !is_valid_key(ar_key) )
            return false;

        this->m_key = ar_key;
        return true;
    }

    /**
     * @brief Is Valid Name
     * 
     * Verilen isim değerinin geçerli olması için
     * en az karakter ve en üst karakter
     * sınırı arasında (dahil) olması gerek
     * 
     * @param string& Name
     * @return bool
     */
    bool Algorithm::is_valid_name(
        const std::string& ar_name
    ) noexcept
    {
        const unsigned int tm_size_name = ar_name.size() * sizeof(char32_t);
        return tm_size_name >= MIN_LEN_NAME && tm_size_name <= MAX_LEN_NAME;
    }

    /**
     * @brief Is Valid Key
     * 
     * Verilen anahtar değerinin geçerli olması için
     * en az karakter ve en üst karakter
     * sınırı arasında (dahil) olması gerek
     * 
     * @param string& Key
     * @return bool
     */
    bool Algorithm::is_valid_key(
        const std::string& ar_key
    ) noexcept
    {
        const unsigned int tm_size_key = ar_key.size() * sizeof(char32_t);
        return tm_size_key >= MIN_LEN_KEY && tm_size_key <= MAX_LEN_KEY;
    }
}