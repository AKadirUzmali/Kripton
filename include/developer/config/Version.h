// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Version
 * 
 * Derleme sürümünü belirtmek için
 * geçerli bir sürüm belirleyici
 * 
 * Major = Büyük Yenilik
 * Minor = Küçük Yenilikler
 * Patch = Hata Düzeltme
 */

// Include:
#include <cstdint>
#include <cassert>
#include <string>
#include <string_view>
#include <array>

// Define:
#ifndef __VERSION_MAJOR__
    #define __VERSION_MAJOR__ 0
#endif

#ifndef __VERSION_MINOR__
    #define __VERSION_MINOR__ 0
#endif

#ifndef __VERSION_PATCH__
    #define __VERSION_PATCH__ 0
#endif

// Namespace:
namespace dev::version
{
    // Using:
    using vermajor_t = uint16_t;
    using verminor_t = uint16_t;
    using verpatch_t = uint32_t;

    // Struct:
    struct version_t
    {
        vermajor_t major;
        verminor_t minor;
        verpatch_t patch : 31;
        bool initialized : 1;
    };

    // Enum:
    enum class build_t : uint8_t
    {
        Dev = 0,
        Alpha,
        Beta,
        Debug,
        Release,
        Null
    };

    inline constexpr size_t size_buildtypes = static_cast<size_t>(build_t::Null) + 1;

    // Array:
    const std::array<std::string, size_buildtypes>
    names = { "Dev", "Alpha", "Beta", "Debug", "Release", "Null" };

    // Class:
    class Version final
    {
        private:
            version_t m_version { 0, 0, 0, false };
            build_t m_build { build_t::Null };

        public:
            explicit Version
            (
                const vermajor_t ver_major = 0,
                const verminor_t ver_minor = 0,
                const verpatch_t ver_patch = 0,
                const build_t ver_build = build_t::Null
            );

            Version(const Version&) = delete;
            Version& operator=(const Version&) = delete;
            Version(Version&&) = delete;
            Version& operator=(Version&&) = delete;

            inline vermajor_t getMajor() const noexcept;
            inline verminor_t getMinor() const noexcept;
            inline verpatch_t getPatch() const noexcept;

            inline build_t getBuildType() const noexcept;

            inline const std::string& strVersion() noexcept;
            inline const std::string& strBuildType() noexcept;
    };

    /**
     * @brief Version
     */
    Version::Version
    (
        const vermajor_t ver_major,
        const verminor_t ver_minor,
        const verpatch_t ver_patch,
        const build_t ver_build
    )
    {
        if( this->m_version.initialized )
            return;

        this->m_version.initialized = true;
        this->m_version.major = ver_major;
        this->m_version.minor = ver_minor;
        this->m_version.patch = ver_patch;

        this->m_build = ver_build;
    }

    /**
     * @brief Get Major
     * 
     * Oluşturulan sürüm durumunun ana sürüm kodu olan
     * Major sürüm numarasını döndürür
     * 
     * @note Çoklu işlem durumlarında dahi sorunsuz çalışır
     * 
     * @return vermajor_t
     */
    vermajor_t Version::getMajor() const noexcept
    {
        return this->m_version.major;
    }

    /**
     * @brief Get Minor
     * 
     * Oluşturulan sürüm durumunun geliştirme sürüm kodu olan
     * Minor sürüm numarasını döndürür
     * 
     * @note Çoklu işlem durumlarında dahi sorunsuz çalışır
     * 
     * @return verminor_t
     */
    verminor_t Version::getMinor() const noexcept
    {
        return this->m_version.minor;
    }

    /**
     * @brief Get Patch
     * 
     * Oluşturulan sürüm durumunun hata düzeltme sürüm kodu olan
     * Patch sürüm numarasını döndürür
     * 
     * @note Çoklu işlem durumlarında dahi sorunsuz çalışır
     * 
     * @return verpatch_t
     */
    verpatch_t Version::getPatch() const noexcept
    {
        return this->m_version.patch;
    }

    /**
     * @brief Get Build Type
     * 
     * Oluşturulan sürüm durumunun türünü belirtir.
     * Hala geliştiriliyor mu yoksa son kullanıcıya ulaşacak
     * şekilde mi vs. bilgisini barındırır
     * 
     * @return build_t
    */
    build_t Version::getBuildType() const noexcept
    {
        return this->m_build;
    }

    /**
     * @brief String Version
     * 
     * Oluşturulan sürüm kodunu metin halinde döndürerek
     * insan tarafından da okunabilir hale getirir ve
     * tek seferde oluştuğu için sonradan ek yük oluşturmaz
     * çünkü hep aynı metini döndürür
     * 
     * @note Çoklu işlem durumlarında dahi sorunsuz çalışır
     * 
     * @return const string&
    */
    const std::string& Version::strVersion() noexcept
    {
        static std::string str_version
        (   'v'+
            std::to_string(this->getMajor()) + '.' +
            std::to_string(this->getMinor()) + '.' +
            std::to_string(this->getPatch())
        );

        return str_version;
    }

    /**
     * @brief String Build Type
     * 
     * Oluştururlan sürüm türünün metin halini
     * döndürerek insan tarafından da okunabilir hale getirir
     * 
     * @note Önceden isimlendirmesi yapılmış sabit metini döndürür
     * 
     * @return const string&
     */
    const std::string& Version::strBuildType() noexcept
    {
        const size_t arr_size = static_cast<size_t>(this->m_build) > names.size() ?
            static_cast<size_t>(build_t::Null) : static_cast<size_t>(this->m_build);

        return names.at(arr_size);
    }
}