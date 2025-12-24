// Abdulkadir U. - 21/12/2025
#pragma once

// Include:
#include <Tool/Utf/Utf.h>

#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <charconv>

// Namespace: Core::Socket
namespace core::socket
{
    // Namespace: Net Packet
    namespace netpacket
    {
        // Limit:
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

    // Using:
    using packetarr_t = std::array<char, SIZE_SOCKET_TOTAL>;

    // Class: NetPacket
    class NetPacket
    {
        static_assert(POS_START_PASSWORD + SIZE_PASSWORD <= SIZE_SOCKET_TOTAL);
        static_assert(POS_START_USERNAME + SIZE_USERNAME <= SIZE_SOCKET_TOTAL);
        static_assert(POS_START_MESSAGE  + SIZE_MESSAGE  <= SIZE_SOCKET_TOTAL);

        private:
            packetarr_t packetdata {};

        public:
            NetPacket() = default;
            
            explicit NetPacket(const std::string&);
            explicit NetPacket(const std::u32string&);

            explicit NetPacket(
                const std::string&,
                const std::string&,
                const std::string&
            );

            explicit NetPacket(
                const std::u32string&,
                const std::u32string&,
                const std::u32string&
            );

            inline size_t getLengthPassword() noexcept;
            inline size_t getLengthUsername() noexcept;
            inline size_t getLengthMessage() noexcept;

            inline std::string getPassword() noexcept;
            inline std::string getUsername() noexcept;
            inline std::string getMessage() noexcept;

            inline packetarr_t& get() noexcept;

            void copy(std::u32string&) noexcept;

            const packetarr_t& pack(const std::string&) noexcept;
            const packetarr_t& pack(const std::u32string&) noexcept;

            const packetarr_t& pack(
                const std::string&,
                const std::string&,
                const std::string&
            ) noexcept;

            const packetarr_t& pack(
                const std::u32string&,
                const std::u32string&,
                const std::u32string&
            ) noexcept;
    };

    /**
     * @brief [Public] Constructor
     * 
     * Şifre, Kullanıcı Adı ve Mesaj verilerini
     * tek parçada olarak içeren UTF-8 string nesnesini
     * alıp netpacket sınıfına uygun olan veriye
     * çevirip kaydedecek
     * 
     * @param std::string& Input
     */
    NetPacket::NetPacket(const std::string& _input)
    {
        this->pack(_input);
    }

    /**
     * @brief [Public] Constructor
     * 
     * Şifre, Kullanıcı Adı ve Mesaj verilerini
     * tek parçada olarak içeren UTF-32 u32string
     * nesnesini alıp netpacket sınıfına uygun olan
     * veriye çevirip kaydedecek
     * 
     * @param std::u32string& Input
     */
    NetPacket::NetPacket(const std::u32string& _input)
    {
        this->pack(_input);
    }

    /**
     * @brief [Public] Constructor
     * 
     * Şifre, kullanıcı adı ve mesaj verilerini
     * tek parçada olarak içeren UTF-8 string nesnesini
     * alıp netpacket sınıfına uygun olan veriye
     * çevirip kaydedecek
     * 
     * @param std::string& Password
     * @param std::string& Nickname
     * @param std::string& Message
     */
    NetPacket::NetPacket(
        const std::string& _passwd,
        const std::string& _nickname,
        const std::string& _msg
    )
    {
        this->pack(_passwd, _nickname, _msg);
    }

    /**
     * @brief [Public] Constructor
     * 
     * Sınıfın ana yapıcısı UTF-32 türündeki veriyi
     * herhangi bir işlem yapmadan direk pack fonksiyonu
     * ile utf-32 olarak işleme alabilecek, kod yazımında
     * kolaylık olması için tasarlandı
     * 
     * @param std::u32string& Password
     * @param std::u32string& Nickname
     * @param std::u32string& Message
     */
    NetPacket::NetPacket(
        const std::u32string& _passwd,
        const std::u32string& _nickname,
        const std::u32string& _msg
    )
    {
        this->pack(_passwd, _nickname, _msg);
    }

    /**
     * @brief [Public] Get Length Password
     * 
     * Paket ile gönderilecek şifre verisinin boyutunu
     * paket içerisinden alıp döndürecek fakat orijinal
     * verinin değil, paketin baş kısmına yerleştirilmiş
     * olan veriyi alacak
     * 
     * @return size_t
     */
    size_t NetPacket::getLengthPassword() noexcept
    {
        const char* ptr_first = reinterpret_cast<const char*>(this->packetdata.data()) + PACK_START_LEN_PASSWORD;
        const char* ptr_last = ptr_first + PACK_LEN_PASSWORD;
        
        size_t len = 0;

        std::from_chars(ptr_first, ptr_last, len);
        return len;
    }

    /**
     * @brief [Public] Get Length Username
     * 
     * Paket ile gönderilecek kullanıcı adı verisinin boyutunu
     * paket içerisinden alıp döndürecek fakat orijinal
     * verinin değil, paketin baş kısmına yerleştirilmiş
     * olan veriyi alacak
     * 
     * @return size_t
     */
    size_t NetPacket::getLengthUsername() noexcept
    {
        const char* ptr_first = reinterpret_cast<const char*>(this->packetdata.data()) + PACK_START_LEN_USERNAME;
        const char* ptr_last = ptr_first + PACK_LEN_USERNAME;
        
        size_t len = 0;

        std::from_chars(ptr_first, ptr_last, len);
        return len;
    }

    /**
     * @brief [Public] Get Length Message
     * 
     * Paket ile gönderilecek mesaj verisinin boyutunu
     * paket içerisinden alıp döndürecek fakat orijinal
     * verinin değil, paketin baş kısmına yerleştirilmiş
     * olan veriyi alacak
     * 
     * @return size_t
     */
    size_t NetPacket::getLengthMessage() noexcept
    {
        const char* ptr_first = reinterpret_cast<const char*>(this->packetdata.data()) + PACK_START_LEN_MESSAGE;
        const char* ptr_last = ptr_first + PACK_LEN_MESSAGE;
        
        size_t len = 0;

        std::from_chars(ptr_first, ptr_last, len);
        return len;
    }

    /**
     * @brief [Public] Get Password
     * 
     * Packet verisi içindeki şifre için ayrılmış
     * alandaki veriyi alıp döndürecek
     * 
     * @return std::string
     */
    std::string NetPacket::getPassword() noexcept
    {
        const size_t len = this->getLengthPassword();
        const char* data = reinterpret_cast<const char*>(this->packetdata.data()) + PACK_START_LEN_PASSWORD;

        return std::string(data, len);
    }

    /**
     * @brief [Public] Get Username
     * 
     * Packet verisi içindeki kullanıcı adı için ayrılmış
     * alandaki veriyi alıp döndürecek
     * 
     * @return std::string
     */
    std::string NetPacket::getUsername() noexcept
    {
        const size_t len = this->getLengthUsername();
        const char* data = reinterpret_cast<const char*>(this->packetdata.data()) + PACK_START_LEN_USERNAME;

        return std::string(data, len);
    }

    /**
     * @brief [Public] Get Message
     * 
     * Packet verisi içindeki mesaj için ayrılmış
     * alandaki veriyi alıp döndürecek
     * 
     * @return std::string
     */
    std::string NetPacket::getMessage() noexcept
    {
        const size_t len = this->getLengthMessage();
        const char* data = reinterpret_cast<const char*>(this->packetdata.data()) + PACK_START_LEN_MESSAGE;

        return std::string(data, len);
    }

    /**
     * @brief [Public] Get
     * 
     * Sınıfa ait şifre, kullanıcı adı ve mesaj verilerini
     * bir array<byte> nesnesi olarak döndürecek fakat bu
     * bellekte fazlaca boyut kaplaması demek, bunun için
     * sınıf içinde oluşturulmuş vektör nesnesini döndürecek
     * bu sayede bellekten tasaruf edilmiş olacak
     * 
     * @return packetarr_t&
     */
    packetarr_t& NetPacket::get() noexcept
    {
        return this->packetdata;
    }

    /**
     * @brief [Public] Copy
     * 
     * packet verisini string veriye çevirir ve bu veriyi
     * belirtilmiş utf-32 değişkene kopyalar
     * 
     * @param std::u32string& Output
    */
    void NetPacket::copy(std::u32string& _output) noexcept
    {
        std::string u8data(reinterpret_cast<const char*>(this->packetdata.data()), SIZE_SOCKET_TOTAL);
        _output = utf::to_utf32(u8data);
    }

    /**
     * @brief [Public] Pack
     * 
     * Şifre, Kullanıcı Adı ve Mesaj verilerini
     * tek parçada olarak içeren UTF-8 string nesnesini
     * alıp netpacket sınıfına uygun olan veriye
     * çevirip kaydedecek
     * 
     * @param std::string& Input
     * @return const packetarr_t&
     */
    const packetarr_t& NetPacket::pack(const std::string& _input) noexcept
    {
        std::fill(this->packetdata.begin(), this->packetdata.end(), char{0});

        const size_t input_size = _input.size();

        const size_t len_passwd = std::min(input_size, SIZE_PASSWORD);
        const size_t len_username = std::min(input_size > POS_START_USERNAME ? input_size - POS_START_USERNAME : 0, SIZE_USERNAME);
        const size_t len_message = std::min(input_size > POS_START_MESSAGE ? input_size - POS_START_MESSAGE : 0, SIZE_MESSAGE);

        std::snprintf(
            this->packetdata.data(), 16, "%03zu%03zu%04zu",
            len_passwd, len_username, len_message
        );

        std::copy_n(
            _input.data() + POS_START_PASSWORD,
            len_passwd,
            this->packetdata.begin() + POS_START_PASSWORD
        );

        std::copy_n(
            _input.data() + POS_START_USERNAME,
            len_username,
            this->packetdata.begin() + POS_START_USERNAME
        );

        std::copy_n(
            _input.data() + POS_START_MESSAGE,
            len_message,
            this->packetdata.begin() + POS_START_MESSAGE
        );

        return this->packetdata;
    }

    /**
     * @brief [Public] Pack
     * 
     * Şifre, Kullanıcı Adı ve Mesaj verilerini
     * tek parçada olarak içeren UTF-32 u32string
     * nesnesini alıp netpacket sınıfına uygun olan
     * veriye çevirip kaydedecek
     * 
     * @param std::u32string& Input
     * @return const packetarr_t&
     */
    const packetarr_t& NetPacket::pack(const std::u32string& _input) noexcept
    {
        std::string u8_input = utf::to_utf8(_input);
        return this->pack(u8_input);
    }

    /**
     * @brief [Public] Pack
     * 
     * Şifre, Kullanıcı Adı ve Mesaj verilerini
     * string referans olarak alıp netpacket sınıfına
     * uygun olan veriye çevirip kaydedecek
     * 
     * @param std::string& Password
     * @param std::string& Nickname
     * @param std::string& Message
     * 
     * @return const packetarr_t&
     */
    const packetarr_t& NetPacket::pack(
        const std::string& _passwd,
        const std::string& _nickname,
        const std::string& _msg
    ) noexcept
    {
        std::fill(this->packetdata.begin(), this->packetdata.end(), char{0});

        const size_t len_passwd = std::min(_passwd.size(), SIZE_PASSWORD);
        const size_t len_username = std::min(_nickname.size(), SIZE_USERNAME);
        const size_t len_message = std::min(_msg.size(), SIZE_MESSAGE);

        std::snprintf(
            this->packetdata.data(), 16, "%03zu%03zu%04zu",
            len_passwd, len_username, len_message
        );

        std::copy_n(
            _passwd.data() + POS_START_PASSWORD,
            len_passwd,
            this->packetdata.begin() + POS_START_PASSWORD
        );

        std::copy_n(
            _nickname.data() + POS_START_USERNAME,
            len_username,
            this->packetdata.begin() + POS_START_USERNAME
        );

        std::copy_n(
            _msg.data() + POS_START_MESSAGE,
            len_message,
            this->packetdata.begin() + POS_START_MESSAGE
        );

        return this->packetdata;
    }

    /**
     * @brief [Public] Pack
     * 
     * Şifre, Kullanıcı Adı ve Mesaj verilerini
     * u32string referans olarak alıp netpacket sınıfına
     * uygun olan veriye çevirip sonra paketleyip kaydedecek
     * 
     * @param std::u32string& Password
     * @param std::u32string& Nickname
     * @param std::u32string& Message
     * 
     * @return const packetarr_t&
     */
    const packetarr_t& NetPacket::pack(
        const std::u32string& _passwd,
        const std::u32string& _nickname,
        const std::u32string& _msg
    ) noexcept
    {
        const std::string tmp_passwd = utf::to_utf8(_passwd);
        const std::string tmp_nickname = utf::to_utf8(_nickname);
        const std::string tmp_msg = utf::to_utf8(_msg);
        
        return this->pack(tmp_passwd, tmp_nickname, tmp_msg);
    }
}