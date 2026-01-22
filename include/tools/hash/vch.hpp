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

// Namespace
namespace tools::hash::vch
{
    // Struct
    struct Vch
    {
        static constexpr std::size_t ss_hash_size = 4;
        static constexpr std::size_t ss_hex_arr_size = 64;

        static constexpr const std::uint64_t ss_seed[ss_hash_size] =
        {
            0xab23c60890abfe23ULL,
            0xec0f26987abb54d1ULL,
            0x3021bdfe925676abULL,
            0x8c234feabcdf2560ULL
        };

        private:
            std::uint64_t m_hash[ss_hash_size] {};
            char m_hexlist[ss_hex_arr_size + 1] {};

            static constexpr char s_hexmap[16] =
            {
                '0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
            };

        private:
            template<std::size_t N>
            constexpr void absorb(const char (&ar_str)[N]) noexcept;

            [[maybe_unused]] constexpr void mix(const std::uint64_t ar_value) noexcept;
            [[maybe_unused]] constexpr void make() noexcept;

        public:
            template<std::size_t N>
            constexpr explicit Vch(
                const char(&ar_contract)[N],
                std::size_t ar_epoch = 0
            ) noexcept
            {
                for(std::size_t tm_count = 0; tm_count < ss_hash_size; ++tm_count)
                    this->m_hash[tm_count] = ss_seed[tm_count];

                this->absorb(ar_contract);
                this->mix(ar_epoch);
                this->make();
            }

            template<std::size_t N>
            constexpr explicit Vch(
                const char(&ar_contract)[N],
                const std::uint64_t (&ar_seed)[ss_hash_size],
                std::size_t ar_epoch = 0
            ) noexcept
            {
                for(std::size_t tm_count = 0; tm_count < ss_hash_size; ++tm_count)
                    this->m_hash[tm_count] = ar_seed[tm_count];

                this->absorb(ar_contract);
                this->mix(ar_epoch);
                this->make();
            }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator==(const Vch& other) const noexcept
            {
                for(std::size_t tm_count = 0; tm_count < ss_hash_size; ++tm_count)
                    if(this->m_hash[tm_count] != other.m_hash[tm_count])
                        return false;
                return true;
            }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator!=(const Vch& other) const noexcept
            { return !(*this == other); }

            [[maybe_unused]] [[nodiscard]] constexpr const char* c_str() const noexcept;
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
    template<std::size_t N>
    constexpr void Vch::absorb(
        const char (&ar_str)[N]
    ) noexcept
    {
        for(std::size_t tm_count = 0; tm_count < N - 1; ++tm_count)
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
     * @param uint64_t Value
     */
    [[maybe_unused]]
    constexpr void Vch::mix(
        const std::uint64_t ar_value
    ) noexcept
    {
        this->m_hash[0] ^= ar_value + (this->m_hash[3] << 6) + (this->m_hash[1] >> 2);
        this->m_hash[1] ^= this->m_hash[0] * 0x2004abd2304cedffULL;
        this->m_hash[2] ^= this->m_hash[1] + (this->m_hash[0] << 4);
        this->m_hash[3] ^= this->m_hash[2] ^ (ar_value >> 3);
    }

    /**
     * @brief Make
     *
     * Bu fonksiyon, m_hash dizisinde tutulan her bir sabit genişlikli
     * (uint64_t) hash parçasını sırayla işler ve her parçayı
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
    [[maybe_unused]]
    constexpr void Vch::make() noexcept
    {
        std::size_t tm_idx = 0;
        
        for(std::size_t tm_count = 0; tm_count < ss_hash_size; ++tm_count) {
            std::uint64_t tm_value = this->m_hash[tm_count];

            for(int tm_by = ss_hex_arr_size - 4; tm_by >= 0; tm_by -= ss_hash_size)
                this->m_hexlist[tm_idx++] = this->s_hexmap[(tm_value >> tm_by) & 0xF];
        }

        this->m_hexlist[ss_hex_arr_size] = '\0';
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
    [[maybe_unused]] [[nodiscard]]
    constexpr const char* Vch::c_str() const noexcept
    {
        return this->m_hexlist;
    }
}