// Abdulkadir U. - 28/10/2025
 
// Include:
#include <sstream>

#include <Algorithm/Algorithm.h>
using namespace core::virbase;

/**
 * @brief [Public] Constructor
 * 
 * Algoritma için gerekli şifreleme anahtarını
 * kayıt edecek ve bu sayede şifrelemeyi sağlayacak.
 * 
 * @param string    Algoritma adı
 * @param u32string Şifreleme anahtarı
 */
Algorithm::Algorithm(const std::string& _name, const std::u32string& _key)
{
    e_algorithm name_status = this->setName(_name);
    e_algorithm key_status = this->setKey(_key);

    switch( name_status )
    {
        case e_algorithm::SUCCESS: break;
        default:
            std::stringstream ss;
            ss << "[Algorithm] Name is not usable and name length can be min" << Algorithm::MIN_NAME_SIZE
                << " and can be max " << Algorithm::MAX_NAME_SIZE << " character";
            throw std::runtime_error(ss.str());
    }

    switch( key_status )
    {
        case e_algorithm::SUCCESS: break;
        case e_algorithm::ERR_KEY_LENGTH_INVALID:
        default:
            std::stringstream ss;
            ss << "[Algorithm] Key is not usable and key length can be min" << Algorithm::MIN_KEY_SIZE
                << " and can be max " << Algorithm::MAX_KEY_SIZE << " character";
            throw std::runtime_error(ss.str());
    }
}

/**
 * @brief [Public] Is Valid Name
 * 
 * Algoritmaya verilen isimli geçerli
 * bir isim mi yoksa değil mi kontrol etsin
 * cevabı döndürsün.
 * 
 * @param string Name
 * @return bool
 */
bool Algorithm::isValidName(const std::string& _name) noexcept
{
    // en küçük ve en büyük boyut arasındaysa geçerlidir.
    return (_name.length() >= Algorithm::MIN_NAME_SIZE && _name.length() <= Algorithm::MAX_NAME_SIZE);
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
    if( !Algorithm::isValidName(_name) )
        return e_algorithm::ERR_NAME_LENGTH_INVALID;

    // isimi değiştirsin
    this->name = _name;
    return (this->name == _name ? e_algorithm::SUCCESS : e_algorithm::ERROR);
}

/**
 * @brief [Public] Is Valid Key
 * 
 * Şifreleme için verilen anahtarın geçerlli
 * olup olmadığını kontrol etsin.
 * 
 * @param u32string Name
 * @return bool
 */
bool Algorithm::isValidKey(const std::u32string& _key) noexcept
{
    // en küçük ve en büyük boyut arasındaysa geçerlidir.
    return (_key.length() >= Algorithm::MIN_KEY_SIZE && _key.length() <= Algorithm::MAX_KEY_SIZE);
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
    if( !Algorithm::isValidKey(_key) )
        return e_algorithm::ERR_KEY_LENGTH_INVALID;

    // anahtarı kopyalayıp değiştirsin
    this->key = _key;
    return (this->key == _key) ? e_algorithm::SUCCESS : e_algorithm::ERROR;
}