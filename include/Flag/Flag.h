// Abdulkadir U. - 31/10/2025
#pragma once

// Include:
#include <cstdint>

// Namespace: Core
namespace core
{
    using flag_t = uint8_t;

    class Flag
    {
        private:
            flag_t value;

        public:
            constexpr Flag (flag_t _value = 0) noexcept : value(_value) {}

            friend constexpr inline Flag operator|(const Flag& _left, const Flag& _right) noexcept;

            constexpr inline Flag& operator|=(const Flag& _other)   noexcept;
            constexpr inline bool operator&(const Flag& _other)     noexcept;

            constexpr inline void set(const flag_t)                     noexcept;
            constexpr inline void add(const flag_t)                     noexcept;
            constexpr inline void remove(const flag_t)                  noexcept;
            constexpr inline void change(const flag_t, const flag_t)    noexcept;

            constexpr inline flag_t get() const noexcept;
    };

    /**
     * @brief [Friend Public] Bitwise Or
     * 
     * Bit türünde iki adet değerin "ya da"
     * eşleşip eşleşmediğini kontrol etsin
     * ama sınıf olmadan da yapabilsin
     * 
     * @param Flag& _left
     * @param Flag& _right
     * 
     * @return Flag
     */
    constexpr Flag operator|(const Flag& _left, const Flag& _right) noexcept
    {
        return Flag(_left.value | _right.value);
    }

    /**
     * @brief [Public] Bitwise Or
     * 
     * Bit türünde iki adet değerin "ya da"
     * eşleşip eşleşmediğini kontrol etsin
     * 
     * @param Flag& Other
     * 
     * @return Flag&
     */
    constexpr Flag& Flag::operator|=(const Flag& _other) noexcept
    {
        this->value |= _other.value;
        return *this;
    }

    /**
     * @brief [Public] Bitwise And
     * 
     * Bit türünde iki adet bit değerin "ve"
     * eşleşip eşleşmediğini kontrol etsin
     * 
     * @param Flag& Other
     * 
     * @return bool
     */
    constexpr bool Flag::operator&(const Flag& _other) noexcept
    {
        return (this->value & _other.value);
    }

    /**
     * @brief [Public] Set
     * 
     * Bayrak değerini ayarlasın
     * 
     * @param flag_t _value
     */
    constexpr void Flag::set(const flag_t _value) noexcept
    {
        this->value = _value;
    }

    /**
     * @brief [Public] Add
     * 
     * Bayrak değerine yenisini eklesin
     * 
     * @param flag_t _value
     */
    constexpr void Flag::add(const flag_t _value) noexcept
    {
        this->value |= _value;
    }

    /**
     * @brief [Public] Remove
     * 
     * Bayrak değerinde varolanı silsin
     * 
     * @param flag_t _value
     */
    constexpr void Flag::remove(const flag_t _value) noexcept
    {
        this->value &= ~_value;
    }

    /**
     * @brief [Public] Change
     * 
     * Bayrak değerinde varolanı silsin
     * yenisini eklesin
     * 
     * @param flag_t _remove
     * @param flag_t _add
     */
    constexpr void Flag::change(const flag_t _remove, const flag_t _add) noexcept
    {
        this->value = (this->value & ~_remove) | _add;
    }

    /**
     * @brief [Public] Get
     * 
     * Bayrak değerini döndürsün
     * 
     * @return flag_t
     */
    constexpr flag_t Flag::get() const noexcept
    {
        return this->value;
    }
}