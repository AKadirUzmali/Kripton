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
            err = 1000,
            err_key_length_invalid,
            err_name_length_invalid,
            err_flag_notchanged,
            err_set_key_fail,
            err_set_name_fail,
            err_flag_invalid_name,
            err_flag_invalid_key,

            succ = 2000,
            succ_flag_default,
            succ_flag_changed,
            succ_set_key,
            succ_set_name,
            succ_flag_valid_name,
            succ_flag_valid_key,

            warn = 3000
        };

        // Flag: Algorithm Status
        inline constexpr flag_t flag_algo_free          { 1 << 0 };
        inline constexpr flag_t flag_algo_error         { 1 << 1 };
        inline constexpr flag_t flag_algo_valid_name    { 1 << 2 };
        inline constexpr flag_t flag_algo_valid_key     { 1 << 3 };
        inline constexpr flag_t flag_algo_idle          { 1 << 4 };
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

        public:
            static inline constexpr size_t MIN_NAME_SIZE = 1;
            static inline constexpr size_t MAX_NAME_SIZE = 32;

            static inline constexpr size_t MIN_KEY_SIZE = 1;
            static inline constexpr size_t MAX_KEY_SIZE = 128;

            explicit Algorithm(const std::string&, const std::u32string&) noexcept;
            ~Algorithm() noexcept;

            virtual inline bool hasError()     const noexcept;
            virtual inline bool hasName()      const noexcept;
            virtual inline bool hasKey()       const noexcept;

            virtual inline bool isIdle()       const noexcept;
            virtual inline bool isActive()     const noexcept;
            
            virtual inline const std::string& getName()    const noexcept;
            virtual inline const std::u32string& getKey()  const noexcept;

            virtual algorithm::e_algorithm setName(const std::string&)     noexcept;
            virtual algorithm::e_algorithm setKey(const std::u32string&)   noexcept;

            virtual bool encrypt(std::u32string&) noexcept = 0;
            virtual bool decrypt(std::u32string&) noexcept = 0;

            virtual bool clear() noexcept;

            static bool checkValidName(const std::string&)     noexcept;
            static bool checkValidKey(const std::u32string&)   noexcept;
    };
}

// Using Namespace:
using namespace core::virbase;
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
: flag(flag_algo_idle)
{
    e_algorithm name_status = this->setName(_name);
    e_algorithm key_status = this->setKey(_key);

    switch( name_status )
    {
        case e_algorithm::succ_flag_valid_name:
            this->flag.set(flag_algo_valid_name);
            break;
        default:
            this->flag.unset(flag_algo_valid_name);
    }

    switch( key_status )
    {
        case e_algorithm::succ_flag_valid_key:
            this->flag.set(flag_algo_valid_key);
            break;
        case e_algorithm::err_key_length_invalid:
        default:
            this->flag.unset(flag_algo_valid_key);
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
    this->flag.clear();
    this->flag.set(flag_algo_free);
}

/**
 * @brief [Private] Set Idle
 * 
 * Bayrağı boş duruma ayarlamak
 */
void Algorithm::setIdle() noexcept
{
    this->flag.set(flag_algo_idle);
}

/**
 * @brief [Private] Set Active
 * 
 * Bayrağı çalışır duruma ayarlamak
 */
void Algorithm::setActive() noexcept
{
    this->flag.unset(flag_algo_idle);
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
    constexpr flag_t error_flags = flag_algo_error;
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
    return static_cast<bool>(this->flag.get() & flag_algo_valid_name);
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
    return static_cast<bool>(this->flag.get() & flag_algo_valid_key);
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
    return static_cast<bool>(this->flag.get() & flag_algo_idle);
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
    return !this->isIdle();
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
    this->name = std::move(_name);

    // eğer hata varsa düzeltsin
    this->flag.set(flag_algo_valid_name);

    return (this->name == _name && this->name.length() == _name.length()) ?
        e_algorithm::succ_set_name :
        e_algorithm::err_set_name_fail;
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
    this->key = std::move(_key);

    // eğer hata varsa düzeltsin
    this->flag.set(flag_algo_valid_key);

    return (this->key == _key && this->key.length() == _key.length()) ?
        e_algorithm::succ_set_key :
        e_algorithm::err_set_key_fail;
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