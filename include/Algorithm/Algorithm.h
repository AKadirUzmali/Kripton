// Abdulkadir U. - 28/10/2025
#pragma once

/**
 * Algorithm (Algoritma)
 * 
 * Kriptoloji de birçok algoritma olabileceği için
 * bu algoritmaların barınabileceği sanal bir sınıf.
 * Şifreleme yöntemi kendi algoritma yöntemine göre
 * veriyi şifreleyecek veya çözecek.
 */

// Include:
#include <cstdint>
#include <string>

#include <Flag/Flag.h>

// Namespace: Core::VirtualBase
namespace core::virbase
{
    // Namespace: Algorithm
    namespace algorithm
    {
        // Enum Class: Algorithm Code
        enum class e_algorithm : size_t
        {
            unknown = 0x0,
            error   = 0x1,
            warning = 0x2,
            success = 0x3,

            err_key_length_invalid = 1000,
            err_name_length_invalid,
            err_flag_notchanged,

            succ_flag_default = 2000,
            succ_flag_changed
        };

        // Flag: Algorithm Status
        inline constexpr flag_t flag_free           { 0 << 0 };
        inline constexpr flag_t flag_notfree        { 1 << 0 };
        inline constexpr flag_t flag_valid_name     { 0 << 1 };
        inline constexpr flag_t flag_valid_key      { 0 << 2 };
        inline constexpr flag_t flag_err_name       { 1 << 1 };
        inline constexpr flag_t flag_err_key        { 1 << 2 };
        inline constexpr flag_t flag_decrypt        { 0 << 3 };
        inline constexpr flag_t flag_encrypt        { 1 << 3 };
        inline constexpr flag_t flag_idle           { 0 << 4 };
        inline constexpr flag_t flag_active         { 1 << 4 };
    }

    // Class: Algorithm
    class Algorithm
    {
        private:
            std::string name;
            std::u32string key;

            Flag flag;

            virtual void setIdle()      noexcept;
            virtual void setActive()    noexcept;
            virtual void setEncrypt()   noexcept;
            virtual void setDecrypt()   noexcept;

        public:
            static inline constexpr size_t MIN_NAME_SIZE = 1;
            static inline constexpr size_t MAX_NAME_SIZE = 32;

            static inline constexpr size_t MIN_KEY_SIZE = 1;
            static inline constexpr size_t MAX_KEY_SIZE = 128;

            explicit Algorithm(const std::string&, const std::u32string&) noexcept;
            ~Algorithm() noexcept;

            virtual bool hasError()     const noexcept;
            virtual bool hasName()      const noexcept;
            virtual bool hasKey()       const noexcept;

            virtual bool isEncrypt()    const noexcept;
            virtual bool isDecrypt()    const noexcept;
            virtual bool isIdle()       const noexcept;
            virtual bool isActive()     const noexcept;
            
            virtual const std::string& getName()    const noexcept;
            virtual const std::u32string& getKey()  const noexcept;

            virtual algorithm::e_algorithm setName(const std::string&)     noexcept;
            virtual algorithm::e_algorithm setKey(const std::u32string&)   noexcept;

            bool encrypt(std::u32string&) noexcept;
            bool decrypt(std::u32string&) noexcept;

            virtual bool clear() noexcept;

            static bool checkValidName(const std::string&)     noexcept;
            static bool checkValidKey(const std::u32string&)   noexcept;

        protected:
            virtual bool doEncrypt(std::u32string&) noexcept { return false; };
            virtual bool doDecrypt(std::u32string&) noexcept { return false; };
    };
}

// Core::Virbase
using namespace core;
using namespace virbase;
using namespace algorithm;

/**
 * @brief [Public] Constructor
 * 
 * Algoritma için gerekli şifreleme anahtarını
 * kayıt edecek ve bu sayede şifrelemeyi sağlayacak.
 * 
 * @param string    Algoritma adı
 * @param u32string Şifreleme anahtarı
 */
Algorithm::Algorithm(const std::string& _name, const std::u32string& _key) noexcept
: flag(flag_idle | flag_notfree | flag_err_name | flag_err_key | flag_encrypt)
{
    e_algorithm name_status = this->setName(_name);
    e_algorithm key_status = this->setKey(_key);

    switch( name_status )
    {
        case e_algorithm::success:
            this->flag.change(flag_err_name, flag_valid_name);
            break;
        default:
            this->flag.change(flag_valid_name, flag_err_name);
    }

    switch( key_status )
    {
        case e_algorithm::success:
            this->flag.change(flag_err_key, flag_valid_key);
            break;
        case e_algorithm::err_key_length_invalid:
        default:
            this->flag.change(flag_valid_key, flag_err_key);
    }
}

/**
 * @brief [Public] Destructor
 * 
 * Algoritma için verilmiş isimi ve
 * anahtar bilgisini geçersiz olması için
 * silecek ve bayrak durumunu da
 * hata durumuna ayarlayacak
 */
Algorithm::~Algorithm() noexcept
{
    this->clear();
    this->flag.change(flag_notfree | flag_valid_name | flag_valid_key | flag_active,
        flag_free | flag_err_name | flag_err_key | flag_idle);
}

/**
 * @brief [Private] Set Idle
 * 
 * Bayrağı boş duruma ayarlamak
 */
void Algorithm::setIdle() noexcept
{
    this->flag.change(flag_active, flag_idle);
}

/**
 * @brief [Private] Set Active
 * 
 * Bayrağı çalışır duruma ayarlamak
 */
void Algorithm::setActive() noexcept
{
    this->flag.change(flag_idle, flag_active);
}

/**
 * @brief [Private] Set Encrypt
 * 
 * Bayrağı şifreleme durumuna almak
 */
void Algorithm::setEncrypt() noexcept
{
    this->flag.change(flag_decrypt, flag_encrypt);
}

/**
 * @brief [Private] Set Decrypt
 * 
 * Bayrağı şifre çözme durumuna almak
 */
void Algorithm::setDecrypt() noexcept
{
    this->flag.change(flag_encrypt, flag_decrypt);
}

/**
 * @brief [Public] Has Error
 * 
 * Bayrak durumuna bakarak bir hata olup olmadığını
 * evet/hayır olarak döndürmemizi sağlayacak
 * 
 * @return bool
 */
bool Algorithm::hasError() const noexcept
{
    constexpr flag_t error_flags = flag_err_name | flag_err_key;
    return static_cast<bool>(this->flag.get() & error_flags);
}

/**
 * @brief [Public] Has Name
 * 
 * Bayrak durumuna bakarak bir algoritma isimi
 * olup olmadığını evet/hayır olarak döndürmemizi sağlayacak
 * 
 * @return bool
 */
bool Algorithm::hasName() const noexcept
{
    return static_cast<bool>(this->flag.get() & flag_valid_name);
}

/**
 * @brief [Public] Has Key
 * 
 * Bayrak durumuna bakarak bir şifreleme anahtarı
 * olup olmadığını evet/hayır olarak döndürmemizi sağlayacak
 * 
 * @return bool
 */
bool Algorithm::hasKey() const noexcept
{
    return static_cast<bool>(this->flag.get() & flag_valid_key);
}

/**
 * @brief [Public] Is Encrypt
 * 
 * Bayrak durumunu inceleyerek şifreleme
 * durumunda olup olmadığını döndürecek
 * 
 * @return bool
 */
bool Algorithm::isEncrypt() const noexcept
{
    return static_cast<bool>(this->flag.get() & flag_encrypt);
}

/**
 * @brief [Public] Is Decrypt
 * 
 * Bayrak durumunu inceleyerek şifre çözme
 * durumunda olup olmadığını döndürecek
 * 
 * @return bool
 */
bool Algorithm::isDecrypt() const noexcept
{
    return static_cast<bool>(this->flag.get() & flag_decrypt);
}

/**
 * @brief [Public] Is Idle
 * 
 * Bayrak durumunu inceleyerek boşta olma
 * durumunda olup olmadığını döndürecek
 * 
 * @return bool
 */
bool Algorithm::isIdle() const noexcept
{
    return static_cast<bool>(this->flag.get() & flag_idle);
}

/**
 * @brief [Public] Is Active
 * 
 * Bayrak durumunu inceleyerek aktif çalışıyor
 * durumunda olup olmadığını döndürecek
 * 
 * @return bool
 */
bool Algorithm::isActive() const noexcept
{
    return static_cast<bool>(this->flag.get() & flag_active);
}

/**
 * @brief [Public] Get Name
 * 
 * Algoritmanın adını genel yazı kullanımına
 * hitap etmesi için string olarak döndürecek
 * 
 * @return string Name
 */
const std::string& Algorithm::getName() const noexcept
{
    return this->name;
}

/**
 * @brief [Public] Get Key
 * 
 * Şifreleme anahtarını getirsin
 * 
 * @return u32string& Key
 */
const std::u32string& Algorithm::getKey() const noexcept
{
    return this->key;
}

/**
 * @brief [Public] Set Name
 * 
 * Algoritmanın adını belirliyoruz
 * ama amacımız daha çok sadece Constructor
 * da kullanmak, bu yüzden gizli fonksiyon.
 * 
 * @param string Name
 * @return e_algorithm Status
 */
e_algorithm Algorithm::setName(const std::string& _name) noexcept
{
    // geçerli bir isim değilse hata döndürsün
    if( !Algorithm::checkValidName(_name) )
        return e_algorithm::err_name_length_invalid;

    // isimi değiştirsin
    this->name = _name;

    // eğer hata varsa düzeltsin
    this->flag.change(flag_err_name, flag_valid_name);

    return (this->name == _name && this->name.length() == _name.length()) ?
        e_algorithm::success :
        e_algorithm::error;
}

/**
 * @brief [Public] Set Key
 * 
 * Algoritmanın anahtarını değiştirmeyi
 * kolaylaştırmak için olan bir fonksiyon.
 * Boşu boşuna yeni sınıf oluşturmayı gerek
 * kalmamasını sağlıyoruz.
 * 
 * @return e_algorithm Status Code
 */
e_algorithm Algorithm::setKey(const std::u32string& _key) noexcept
{
    // geçerli bir anahtar değilse hata döndürsün
    if( !Algorithm::checkValidKey(_key) )
        return e_algorithm::err_key_length_invalid;

    // anahtarı kopyalayıp değiştirsin
    this->key = _key;

    // eğer hata varsa düzeltsin
    this->flag.change(flag_err_key, flag_valid_key);

    return (this->key == _key && this->key.length() == _key.length()) ?
        e_algorithm::success :
        e_algorithm::error;
}

/**
 * @brief [Public] Encrypt
 * 
 * Şifreleme yapmayı sağlayacak fonksiyon
 * ve güvenlik kontrollü olabilmesi için
 * ana sınıf içinde değiştirilemez olacak
 * ve güvenlik kısmından sonra asıl şifreleme
 * çalışması için ayrılmış olan fonksiyon çalışacak
 * 
 * @param u32string& Text
 * @return bool
 */
bool Algorithm::encrypt(std::u32string& _text) noexcept
{
    if( !Algorithm::checkValidName(this->getName()) )
        this->flag.change(flag_valid_name, flag_err_name);

    if( !Algorithm::checkValidKey(this->getKey()) )
        this->flag.change(flag_valid_key, flag_err_key);

    this->flag.change(flag_decrypt, flag_encrypt);

    return this->hasError() ?
        false :
        this->doEncrypt(_text) && !this->hasError();
}

/**
 * @brief [Public] Decrypt
 * 
 * Şifre çözmeyi sağlayacak fonksiyon
 * ve güvenlik kontrollü olabilmesi için
 * ana sınıf içinde değiştirilemez olacak
 * ve güvenlik kısmından sonra asıl şifre çözücünün
 * çalışması için ayrılmış olan fonksiyon çalışacak
 * 
 * @param u32string& Text
 * @return bool
 */
bool Algorithm::decrypt(std::u32string& _text) noexcept
{
    if( !Algorithm::checkValidName(this->getName()) )
        this->flag.change(flag_valid_name, flag_err_name);

    if( !Algorithm::checkValidKey(this->getKey()) )
        this->flag.change(flag_valid_key, flag_err_key);

    this->flag.change(flag_encrypt, flag_decrypt);

    return this->hasError() ?
        false :
        this->doDecrypt(_text) && !this->hasError();
}

/**
 * @brief [Public] Clear
 * 
 * Sınıftaki verilen temizlenmesini
 * sağlayarak sınıftaki değişkenleri boş bırakmak
 * 
 * @return bool
 */
bool Algorithm::clear() noexcept
{
    this->key = U"";
    this->name = "";

    return true;
}

/**
 * @brief [Static Public] Check Is Valid Name
 * 
 * Algoritmaya verilen isimli geçerli
 * bir isim mi yoksa değil mi kontrol etsin
 * cevabı döndürsün.
 * 
 * @param string Name
 * @return bool
 */
bool Algorithm::checkValidName(const std::string& _name) noexcept
{
    // en küçük ve en büyük boyut arasındaysa geçerlidir.
    return (_name.length() >= Algorithm::MIN_NAME_SIZE && _name.length() <= Algorithm::MAX_NAME_SIZE);
}

/**
 * @brief [Static Public] Check Is Valid Key
 * 
 * Şifreleme için verilen anahtarın geçerlli
 * olup olmadığını kontrol etsin.
 * 
 * @param u32string Name
 * @return bool
 */
bool Algorithm::checkValidKey(const std::u32string& _key) noexcept
{
    // en küçük ve en büyük boyut arasındaysa geçerlidir.
    return (_key.length() >= Algorithm::MIN_KEY_SIZE && _key.length() <= Algorithm::MAX_KEY_SIZE);
}