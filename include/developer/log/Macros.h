// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Macros
 * 
 * Geliştirici işlemlerinde makroları kullanarak
 * daha iyi ve düzgün kayıt yapabilmek adına
 * oluşturuyoruz
 */

// Include:
#include <developer/core/Source.h>
#include <developer/log/Logger.h>
#include <developer/test/Expect.h>
#include <developer/test/Assert.h>

// Define:
#if defined(__DEVELOPER__) && __DEVELOPER__
    #define LOG(msg) \
        dev::log::Logger::write(dev::level::Level::Info, \
            msg, \
            dev::source::Source{__FILE__, __func__, __LINE__})

    #define LOG_MSG(type, msg) \
        dev::log::Logger::write(static_cast<dev::level::Level>(type), \
        msg, \
        dev::source::Source{__FILE__, __func__, __LINE__})

    #define EXPECT_MSG(result, msg) \
        do { \
            dev::log::Logger::write(result ? \
                dev::level::Level::Succ : dev::level::Level::Err, \
                msg, \
                dev::source::Source{__FILE__, __func__, __LINE__} \
            ); \
        } while(0)

    #define ASSERT_MSG(result, msg) \
        do { \
            if( !result ) { \
                dev::log::Logger::write(dev::level::Level::Err, \
                    msg, \
                    dev::source::Source{__FILE__, __func__, __LINE__}); \
                dev::test::kill(); \
            } else { \
                dev::log::Logger::write(dev::level::Level::Succ, \
                    msg, \
                    dev::source::Source{__FILE__, __func__, __LINE__}); \
            } \
        } while(0)
#else
    #define LOG(msg) ((void)sizeof(msg))
    #define LOG_MSG(type, msg) ((void)sizeof(type))

    #define EXPECT_MSG(result, msg) ((void)sizeof(result))
    #define ASSERT_MSG(result, msg) ((void)sizeof(result))
#endif