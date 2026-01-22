// Abdulkadir U. - 12/01/2026
// Abdulkadir U. - 22/01/2026
#pragma once

/**
 * Macros
 * 
 * Geliştirici işlemlerinde makroları kullanarak
 * daha iyi ve düzgün kayıt yapabilmek adına
 * oluşturuyoruz
 */

// Include
#include <dev/core/source.hpp>
#include <dev/test/expect.hpp>
#include <dev/test/assert.hpp>
#include <dev/log/levels.hpp>
#include <dev/log/logger.hpp>

// Using Namespace
using namespace dev;

// Define
#if __DEVELOPER__
    #define LOG(ar_logger, ar_msg) \
        ar_logger.write(level::level_t::Info, \
            ar_msg, \
            source::Source{__FILE__, __func__, __LINE__})

    #define LOG_MSG(ar_logger, ar_type, ar_msg) \
        ar_logger.write(static_cast<level::level_t>(ar_type), \
        ar_msg, \
        source::Source{__FILE__, __func__, __LINE__})

    #define EXPECT_MSG(ar_logger, ar_result, ar_msg) \
        do { \
            ar_logger.write(result ? \
                level::level_t::Succ : level::level_t::Err, \
                ar_msg, \
                source::Source{__FILE__, __func__, __LINE__} \
            ); \
        } while(0)

    #define ASSERT_MSG(ar_logger, ar_result, ar_msg) \
        do { \
            if( !result ) { \
                ar_logger.write(level::level_t::Err, \
                    ar_msg, \
                    source::Source{__FILE__, __func__, __LINE__}); \
                dev::test::kill(); \
            } else { \
                ar_logger.write(level::level_t::Succ, \
                    ar_msg, \
                    source::Source{__FILE__, __func__, __LINE__}); \
            } \
        } while(0)
#else
    #define LOG(ar_logger, ar_msg) ((void)0)
    #define LOG_MSG(ar_logger, ar_type, ar_msg) ((void)0)

    #define EXPECT_MSG(ar_logger, ar_result, ar_msg) ((void)0)
    #define ASSERT_MSG(ar_logger, ar_result, ar_msg) ((void)0)
#endif