// Abdulkadir U. - 22/10/2025
#pragma once

// C++
#ifndef __cplusplus
    #error "[PRE ERROR] C++ Required"
#endif

/*
 * Singleton
 * 
 * Tek seferlik oluşturulup yönetilmesi gereken sınıflar
 * Singleton yönetim şekli için uygundur. Düzenli bir yapı
 * için bir Singleton sınıfı oluşturup bunu ihtiyaç duyan
 * sınıflarda kullanacağız
*/

// Namespace: Pattern
class Pattern
{
    // Class: Singleton
    template <class TClass>
    class Singleton
    {
        private:
            static TClass self;

        public:
            static TClass& get();

        protected:
            Singleton() = delete;
            Singleton(const Singleton&) = delete;
            Singleton& operator=(const Singleton&) = delete;

            ~Singleton() = default;
    };

    // Statik Nesne Tanımı
    template <class TClass>
    TClass Singleton<TClass>::self {};

    /**
     * @brief [Public] Get
     * 
     * Sınıf objesinin referansını döndürür ve
     * bu sayede pointer kullanmaya gerek kalmaz
     * ve bellek açıkları oluşmaz
     * 
     * @return Self
     */
    template <class TClass>
    TClass& Singleton<TClass>::get()
    {
        return self;
    }
}