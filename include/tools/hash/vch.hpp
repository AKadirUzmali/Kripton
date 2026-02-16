// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Version Contract Hash (VCH)
 * 
 * Sürüm numarası veya değerlerini tersine mühendislik ile
 * değiştirmek gayet kolay olacaktır. Bunun yerine ayarlanmış
 * bir sözleşme metinini belirlenen yöntem ile şifreleyerek
 * karşılaştırma yaparak daha etkili bir sürüm
 * kontrolü sağlanmış olur. Önemli değişkenlerin sakladığı değerler
 * şifreleme için kullanılacaktır.
 */

// Include
#include <cstdint>
#include <cstddef>
#include <array>

// Namespace
namespace tools::hash::vch
{
    // Struct
    template<uint8_t HashSizeT>
    struct Vch
    {
        private:
            static constexpr uint8_t ss_hash_size = 4;
            static constexpr const uint32_t ss_seed[ss_hash_size] =
            {
                0xab23c608UL,
                0xec0f2698UL,
                0x3021bdfeUL,
                0x8c234feeUL
            };

            static constexpr uint8_t ss_hex_arr_size = (HashSizeT) % (sizeof(uint32_t) * 8);

            std::array<uint32_t, ss_hash_size> m_hash {};
            std::array<char, ss_hex_arr_size + 1> m_hexlist {};

            static constexpr char s_hexmap[16] =
            {
                '0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
            };

        private:
            template<uint32_t N>
            constexpr void absorb(const char (&ar_str)[N]) noexcept;

            constexpr void mix(const uint32_t ar_value) noexcept;
            constexpr void make() noexcept;

        public:
            template<uint32_t N>
            constexpr explicit Vch(
                const char(&ar_contract)[N],
                uint32_t ar_epoch = 0
            ) noexcept
            {
                for(uint32_t tm_count = 0; tm_count < this->m_hash.size(); ++tm_count)
                    this->m_hash.at(tm_count) = ss_seed[tm_count];

                this->absorb(ar_contract);
                this->mix(ar_epoch);
                this->make();
            }

            constexpr bool operator==(const Vch& other) const noexcept
            {
                for(uint32_t tm_count = 0; tm_count < this->m_hash.size(); ++tm_count)
                    if(this->m_hash.at(tm_count) != other.m_hash.at(tm_count))
                        return false;
                return true;
            }

            constexpr bool operator!=(const Vch& other) const noexcept
            { return !(*this == other); }

            constexpr const char* c_str() const noexcept;
    };

    /**
     * @brief Absorb
     * 
     * Alınan metini belirtilen boyut kadar
     * dönerek veriyi karıştırır ve daha karmaşık ve
     * özel bir metin olmasını sağlar
     * 
     * @param char[N] Text
     */
    template<uint8_t HashSizeT>
    template<uint32_t N>
    constexpr void Vch<HashSizeT>::absorb(
        const char (&ar_str)[N]
    ) noexcept
    {
        for(uint32_t tm_count = 0; tm_count < N - 1; ++tm_count)
            this->mix(static_cast<std::uint8_t>(ar_str[tm_count]));
    }

    /**
     * @brief Mix
     * 
     * Verileri verilen değere göre karıştırmayı sağlayacak
     * ve ek olarak ikinci veriyi rastgele belirlediğimiz bir değer ile
     * çarpıyoruz daha benzersiz olması için. Sırasıyla işlemleri yaptıktan
     * sonra yeni karışım olmuş oluyor
     * 
     * @param uint32_t Value
     */
    template<uint8_t HashSizeT>
    constexpr void Vch<HashSizeT>::mix(
        const uint32_t ar_value
    ) noexcept
    {
        this->m_hash.at(0) ^= ar_value + (this->m_hash.at(3) << 6) + (this->m_hash.at(1) >> 2);
        this->m_hash.at(1) ^= this->m_hash.at(0) * 0x2004abd2304cedffULL;
        this->m_hash.at(2) ^= this->m_hash.at(1) + (this->m_hash.at(0) << 4);
        this->m_hash.at(3) ^= this->m_hash.at(2) ^ (ar_value >> 3);
    }

    /**
     * @brief Make
     *
     * Bu fonksiyon, m_hash dizisinde tutulan her bir sabit genişlikli
     * (uint32_t) hash parçasını sırayla işler ve her parçayı
     * hexadecimal karakterlere dönüştürerek m_hexlist dizisine yazar.
     *
     * Dış döngü, hash durumunu oluşturan her 64-bit bloğu sırayla alır.
     * İç döngü ise bu 64-bit değeri, en anlamlı nibble’dan (4 bit)
     * başlayarak 4 bitlik adımlarla sağa kaydırır.
     *
     * Her adımda elde edilen 4 bitlik değer (0–15 aralığında),
     * m_hexmap tablosu kullanılarak karşılık gelen hexadecimal
     * karaktere çevrilir.
     *
     * tm_idx sayacı, üretilen tüm hexadecimal karakterlerin
     * m_hexlist dizisine kesintisiz ve sıralı biçimde yazılmasını sağlar.
     *
     * İşlem tamamlandığında m_hexlist null karakter ile sonlandırılır
     * ve sonuç C-string olarak güvenle kullanılabilir.
     *
     * Bu dönüşüm:
     *  - Mimari bağımsızdır (32-bit / 64-bit fark etmez)
     *  - Deterministiktir (aynı hash → aynı çıktı)
     *  - Sadece temsil (encoding) yapar, ek karmaşıklık eklemez
     */
    template<uint8_t HashSizeT>
    constexpr void Vch<HashSizeT>::make() noexcept
    {
        uint32_t tm_idx = 0;
        
        for(uint32_t tm_count = 0; tm_count < ss_hash_size; ++tm_count) {
            uint32_t tm_value = this->m_hash.at(tm_count);

            for(int tm_by = ss_hex_arr_size - 4; tm_by >= 0; tm_by -= ss_hash_size)
                this->m_hexlist.at(tm_idx++) = this->s_hexmap[(tm_value >> tm_by) & 0xF];
        }

        this->m_hexlist.at(ss_hex_arr_size) = '\0';
    }

    /**
     * @brief C Str
     * 
     * Oluşturulan Hexadecimal hash'i derleme aşamasında
     * ayarladığımız için direk adres bilgisini döndürüyoruz
     * bu sayede performanslı bir şekilde veri erişimi yapılır
     * 
     * @return const char*
     */
    template<uint8_t HashSizeT>
    constexpr const char* Vch<HashSizeT>::c_str() const noexcept
    {
        return this->m_hexlist.data();
    }
}