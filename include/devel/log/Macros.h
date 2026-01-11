// Abdulkadir U. - 11/01/2026
#pragma once

/**
 * Macros
 * 
 * Geliştirici işlemlerinde makroları kullanarak
 * daha iyi ve düzgün kayıt yapabilmek adına
 * oluşturuyoruz
 */

// Include:
#include <devel/log/Logger.h>
#include <devel/test/Expect.h>
#include <devel/test/Assert.h>

// Define:
#if defined(__DEVELOPER__) && __DEVELOPER__
    #define LOG(msg) \
        ::devel::log::Logger::write( ::devel::log::Level::Info, msg)

    #define LOG_MSG(type, msg) \
        ::devel::log::Logger::write(static_cast<::devel::log::Level>(type), msg)

    #define EXPECT_MSG(result, msg) \
        do { \
            ::devel::log::Logger::write(result ? \
                ::devel::log::Level::Succ : ::devel::log::Level::Err, \
                msg \
            ); \
        } while(0)

    #define ASSERT_MSG(result, msg) \
        do { \
            if( !result ) { \
                ::devel::log::Logger::write(::devel::log::Level::Err, msg); \
                ::devel::test::kill(); \
            } else { \
                ::devel::log::Logger::write(::devel::log::Level::Succ, msg); \
            } \
        } while(0)
#else
    #define LOG(msg) ((void)0)
    #define LOG_MSG(type, msg) ((void)sizeof(type))

    #define EXPECT_MSG(result, msg) ((void)sizeof(result))
    #define ASSERT_MSG(result, msg) ((void)sizeof(result))
#endif