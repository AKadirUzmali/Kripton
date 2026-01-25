// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Assert
 * 
 * Test sırasında beklenen sonucu direk döndürmek
 * yerine, istenen durum oluşmamasında yıkım yaparak
 * kendisini gösterir. Bu ise normal karşılaştırmalar
 * yerine, beklenenin olmaması durumunda gelecekte
 * hatalar oluşmaması gereken durumlar için kullanılır
 * 
 * Örnek: Çocuk doğmamışsa yemek yiyemez
 * 
 * Bu örnekte de olduğu gibi, eğer çocuk doğmamışsa yemek
 * yiyemez ama bu hata kontrolü olmasaydı eğer, çocuk
 * doğmasa bile yemek yedirmeyi deneyecekti sistem ve
 * ya hata ya da beklenmeyen bir sonuç ile devam edecekti
 */

// Include
#include <cstdlib>

// Namespace
namespace dev::test
{
    /**
     * @brief Kill
     * 
     * Programı sonlandırmayı sağlar
     */
    [[noreturn]]
    inline void kill() noexcept
    {
        std::exit(EXIT_FAILURE);
    }

    /**
     * @brief Assert Equal
     * 
     * İki adet bilinmeyen tür değişkeni alınır ve
     * karşılaştırılır. Eğer geliştirici modunda ise
     * karşılaştırır ve eğer eşit değiller ise, programı
     * sonlandırmayı sağlar, aksi halde herhangi bir şey olmaz
     * 
     * @note Sadece geliştirici modu için
     * 
     * @tparam L Left
     * @tparam R Right
     */
    template<typename L, typename R>
    inline void assert_eq(const L& l, const R& r)
    {
        if( l != r )
            kill();
    }
}