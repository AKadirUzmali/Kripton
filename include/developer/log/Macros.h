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
#include <developer/log/Logger.h>

// Define:
#if defined(__DEVELOPER__) && __DEVELOPER__
    #define LOG(logger, msg) \
        logger.write(dev::level::Level::Info, \
            msg, \
            dev::source::Source{__FILE__, __func__, __LINE__})

    #define LOG_MSG(logger, type, msg) \
        logger.write(static_cast<dev::level::Level>(type), \
        msg, \
        dev::source::Source{__FILE__, __func__, __LINE__})

    #define EXPECT_MSG(logger, result, msg) \
        do { \
            logger.write(result ? \
                dev::level::Level::Succ : dev::level::Level::Err, \
                msg, \
                dev::source::Source{__FILE__, __func__, __LINE__} \
            ); \
        } while(0)

    #define ASSERT_MSG(logger, result, msg) \
        do { \
            if( !result ) { \
                logger.write(dev::level::Level::Err, \
                    msg, \
                    dev::source::Source{__FILE__, __func__, __LINE__}); \
                dev::test::kill(); \
            } else { \
                logger.write(dev::level::Level::Succ, \
                    msg, \
                    dev::source::Source{__FILE__, __func__, __LINE__}); \
            } \
        } while(0)
#else
    #define LOG(logger, msg) ((void)sizeof(logger))
    #define LOG_MSG(logger, type, msg) ((void)sizeof(logger))

    #define EXPECT_MSG(logger, result, msg) ((void)sizeof(logger))
    #define ASSERT_MSG(logger, result, msg) ((void)sizeof(logger))
#endif