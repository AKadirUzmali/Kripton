// Abdulkadir U. - 28/10/2025
 
// Include:
#include <sstream>

#include <Algorithm/Algorithm.h>
using namespace core::virbase;
using namespace core::virbase::algorithm;

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
    this->name = "";
    this->key = U"";
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