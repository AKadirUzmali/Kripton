// Abdulkadir U. - 2026/10/07
#pragma once

/**
 * Socket Pool (Soket Havuzu)
 * 
 * Sokete ait tüm dosyaları tek bir dosyada tutmayı
 * sağlıyoruz bu sayede karmaşıklık daha az oluyor
 */

// Include
#include <socket/socket.hpp>
#include <socket/netpacket.hpp>
#include <socket/policy.hpp>
#include <socket/server/server.hpp>
#include <socket/client/client.hpp>

// Namespace
namespace pool::socketpool
{
    // Using Namespace
    using namespace netsocket;
}