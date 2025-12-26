// Abdulkadir U. - 21/12/2025
#pragma once

// Include:
#include <Tool/Utf/Utf.h>

#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <charconv>

// Namespace: Core::Virbase::Socket
namespace core::virbase::socket
{
    // Namespace: Net Packet
    namespace netpacket
    {
        // Limit:
        static inline constexpr size_t SIZE_HEADER = 10;

        static inline constexpr size_t SIZE_PASSWORD = 384;
        static inline constexpr size_t SIZE_USERNAME = 128;
        static inline constexpr size_t SIZE_MESSAGE = 2048;

        static inline constexpr size_t PACK_LEN_PASSWORD = 3;
        static inline constexpr size_t PACK_LEN_USERNAME = 3;
        static inline constexpr size_t PACK_LEN_MESSAGE = 4;

        static inline constexpr size_t PACK_START_LEN_PASSWORD = 0;
        static inline constexpr size_t PACK_START_LEN_USERNAME = PACK_START_LEN_PASSWORD + PACK_LEN_PASSWORD;
        static inline constexpr size_t PACK_START_LEN_MESSAGE = PACK_START_LEN_USERNAME + PACK_LEN_USERNAME;

        static inline constexpr size_t PACK_LEN_TOTAL = PACK_LEN_PASSWORD + PACK_LEN_USERNAME + PACK_LEN_MESSAGE;
        static inline constexpr size_t PACK_START_LEN_TOTAL = PACK_LEN_PASSWORD + PACK_LEN_USERNAME + PACK_LEN_MESSAGE;
        static inline constexpr size_t SIZE_SOCKET_TOTAL = PACK_START_LEN_TOTAL + SIZE_PASSWORD + SIZE_USERNAME + SIZE_MESSAGE;

        static inline constexpr size_t SIZE_OVER_SOCKET = SIZE_SOCKET_TOTAL + 128;
        static inline constexpr size_t SIZE_UNDER_SOCKET = SIZE_PASSWORD + SIZE_USERNAME;

        static inline constexpr size_t POS_START_PASSWORD = PACK_START_LEN_TOTAL;
        static inline constexpr size_t POS_END_PASSWORD = POS_START_PASSWORD + SIZE_PASSWORD;

        static inline constexpr size_t POS_START_USERNAME = POS_END_PASSWORD;
        static inline constexpr size_t POS_END_USERNAME = POS_START_USERNAME + SIZE_USERNAME;

        static inline constexpr size_t POS_START_MESSAGE = POS_END_USERNAME;
        static inline constexpr size_t POS_END_MESSAGE = POS_START_MESSAGE + SIZE_MESSAGE;
    }

    // Using Namespace:
    using namespace netpacket;
    using namespace tool;

    // Class: NetPacket
    class NetPacket
    {
        static_assert(POS_START_PASSWORD + SIZE_PASSWORD <= SIZE_SOCKET_TOTAL);
        static_assert(POS_START_USERNAME + SIZE_USERNAME <= SIZE_SOCKET_TOTAL);
        static_assert(POS_START_MESSAGE + SIZE_MESSAGE <= SIZE_SOCKET_TOTAL);

        private:
            std::string password;
            std::string username;
            std::string message;

            std::vector<std::byte> buffer;

            static inline void append_bytes(
                std::vector<std::byte>& _out,
                const char* _data,
                std::size_t _len
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
        std::size_t _len
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

        char header[SIZE_HEADER + 1] {};

        std::snprintf(
            header,
            sizeof(header),
            "%03zu%03zu%04zu",
            _u8_pwd.size(),
            _u8_usrname.size(),
            _u8_msg.size()
        );

        append_bytes(this->buffer, header, SIZE_HEADER);

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