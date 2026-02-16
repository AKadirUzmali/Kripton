// Abdulkadir U. - 21/12/2025
// Abdulkadir U. - 2026/02/09
#pragma once

// Include
#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <charconv>

#include <kits/toolkit.hpp>

// Namespace
namespace netsocket::netpacket
{
    static inline constexpr uint32_t _SIZE_HEADER = 10;

    static inline constexpr uint32_t _SIZE_PASSWORD = 384;
    static inline constexpr uint32_t _SIZE_USERNAME = 128;
    static inline constexpr uint32_t _SIZE_MESSAGE = 2048;

    static inline constexpr uint32_t _PACK_LEN_PASSWORD = 3;
    static inline constexpr uint32_t _PACK_LEN_USERNAME = 3;
    static inline constexpr uint32_t _PACK_LEN_MESSAGE = 4;

    static inline constexpr uint32_t _PACK_START_LEN_PASSWORD = 0;
    static inline constexpr uint32_t _PACK_START_LEN_USERNAME = _PACK_START_LEN_PASSWORD + _PACK_LEN_PASSWORD;
    static inline constexpr uint32_t _PACK_START_LEN_MESSAGE = _PACK_START_LEN_USERNAME + _PACK_LEN_USERNAME;

    static inline constexpr uint32_t _PACK_LEN_TOTAL = _PACK_LEN_PASSWORD + _PACK_LEN_USERNAME + _PACK_LEN_MESSAGE;
    static inline constexpr uint32_t _PACK_START_LEN_TOTAL = _PACK_LEN_PASSWORD + _PACK_LEN_USERNAME + _PACK_LEN_MESSAGE;
    static inline constexpr uint32_t _SIZE_SOCKET_TOTAL = _PACK_START_LEN_TOTAL + _SIZE_PASSWORD + _SIZE_USERNAME + _SIZE_MESSAGE;

    static inline constexpr uint32_t _SIZE_OVER_SOCKET = _SIZE_SOCKET_TOTAL + 128;
    static inline constexpr uint32_t _SIZE_UNDER_SOCKET = _SIZE_PASSWORD + _SIZE_USERNAME;

    static inline constexpr uint32_t _POS_START_PASSWORD = _PACK_START_LEN_TOTAL;
    static inline constexpr uint32_t _POS_END_PASSWORD = _POS_START_PASSWORD + _SIZE_PASSWORD;

    static inline constexpr uint32_t _POS_START_USERNAME = _POS_END_PASSWORD;
    static inline constexpr uint32_t _POS_END_USERNAME = _POS_START_USERNAME + _SIZE_USERNAME;

    static inline constexpr uint32_t _POS_START_MESSAGE = _POS_END_USERNAME;
    static inline constexpr uint32_t _POS_END_MESSAGE = _POS_START_MESSAGE + _SIZE_MESSAGE;

    // Using Namespace
    using namespace tools::charset;

    // Class
    class NetPacket final
    {
        static_assert(_POS_START_PASSWORD + _SIZE_PASSWORD <= _SIZE_SOCKET_TOTAL);
        static_assert(_POS_START_USERNAME + _SIZE_USERNAME <= _SIZE_SOCKET_TOTAL);
        static_assert(_POS_START_MESSAGE + _SIZE_MESSAGE <= _SIZE_SOCKET_TOTAL);

        private:
            std::string password;
            std::string username;
            std::string message;

            std::vector<std::byte> buffer;

            static inline void append_bytes(
                std::vector<std::byte>& _out,
                const char* _data,
                std::uint32_t _len
            ) noexcept;

        public:
            NetPacket(
                const std::string& _u8_pwd,
                const std::string& _u8_usrname,
                const std::string& _u8_msg
            );

            NetPacket(
                const std::u32string& _u32_pwd,
                const std::u32string& _u32_usrname,
                const std::u32string& _u32_msg
            );

            bool empty() const noexcept;

            inline const std::vector<std::byte>& get() const noexcept;

            inline const std::string& getPassword() const noexcept;
            inline const std::string& getUsername() const noexcept;
            inline const std::string& getMessage() const noexcept;

            const std::vector<std::byte>& pack(
                const std::string& _u8_pwd,
                const std::string& _u8_usrname,
                const std::string& _u8_msg
            ) noexcept;

            const std::vector<std::byte>& pack(
                const std::u32string& _u32_pwd,
                const std::u32string& _u32_usrname,
                const std::u32string& _u32_msg
            ) noexcept;
    };

    void NetPacket::append_bytes(
        std::vector<std::byte>& _out,
        const char* _data,
        std::uint32_t _len
    ) noexcept
    {
        const std::byte* b = reinterpret_cast<const std::byte*>(_data);
        _out.insert(_out.end(), b, b + _len);
    }

    NetPacket::NetPacket(
        const std::string& _u8_pwd,
        const std::string& _u8_usrname,
        const std::string& _u8_msg
    )
    {
        this->pack(_u8_pwd, _u8_usrname, _u8_msg);
    }

    NetPacket::NetPacket(
        const std::u32string& _u32_pwd,
        const std::u32string& _u32_usrname,
        const std::u32string& _u32_msg
    )
    {
        this->pack(_u32_pwd, _u32_usrname, _u32_msg);
    }

    bool NetPacket::empty(
    ) const noexcept
    {
        return this->username.empty() || this->message.empty();
    }

    const std::vector<std::byte>& NetPacket::get(
    ) const noexcept
    {
        return this->buffer;
    }

    const std::string& NetPacket::getPassword(
    ) const noexcept
    {
        return this->password;
    }

    const std::string& NetPacket::getUsername(
    ) const noexcept
    {
        return this->username;
    }

    const std::string& NetPacket::getMessage(
    ) const noexcept
    {
        return this->message;
    }

    const std::vector<std::byte>& NetPacket::pack(
        const std::string& _u8_pwd,
        const std::string& _u8_usrname,
        const std::string& _u8_msg
    ) noexcept
    {
        this->password.clear();
        this->username.clear();
        this->message.clear();
        this->buffer.clear();

        if( _u8_usrname.empty() || _u8_msg.empty() )
            return this->buffer;

        char header[_SIZE_HEADER + 1] {};

        std::snprintf(
            header,
            sizeof(header),
            "%03zu%03zu%04zu",
            _u8_pwd.size(),
            _u8_usrname.size(),
            _u8_msg.size()
        );

        append_bytes(this->buffer, header, _SIZE_HEADER);

        append_bytes(this->buffer, _u8_pwd.data(), _u8_pwd.size());
        append_bytes(this->buffer, _u8_usrname.data(), _u8_usrname.size());
        append_bytes(this->buffer, _u8_msg.data(), _u8_msg.size());

        this->password = _u8_pwd;
        this->username = _u8_usrname;
        this->message = _u8_msg;

        return this->buffer;
    }

    const std::vector<std::byte>& NetPacket::pack(
        const std::u32string& _u32_pwd,
        const std::u32string& _u32_usrname,
        const std::u32string& _u32_msg
    ) noexcept
    {
        return this->pack(
            utf::to_utf8(_u32_pwd),
            utf::to_utf8(_u32_usrname),
            utf::to_utf8(_u32_msg)
        );
    }
}