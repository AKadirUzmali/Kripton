// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Developer
 * 
 * Geliştirici için gerekli olanların içe aktarıldığı
 * genel bir dosya. Karmaşıklığı önlemek için vardır.
 * Bazı dosyalar diğerini içerdiği için o dosyalar
 * eklenmeyebilir fakat gelecekte değişiklikler olabilir
 * ve bu dosyada sürekli değişim yapmamak adına
 * var olan geliştirici dosyalarının hepsi içe aktarılıyor.
 * 
 * Bu sayede zaten içe aktarılmış olan dosyalar diğer dosya
 * içinde olsa bile içe aktarılmayı engelleyen derleyici
 * yapısı sebebiyle tekrar tekrar içe aktarma hatalarını da
 * önlemiş olacak
 */

// Include
#include <dev/log/levels.hpp>
#include <dev/log/logger.hpp>

#include <dev/trace/timeline.hpp>
#include <dev/trace/trace.hpp>

#include <dev/test/expect.hpp>
#include <dev/test/assert.hpp>

#include <kits/toolkit.hpp>
#include <kits/outputkit.hpp>
#include <kits/corekit.hpp>

#include <dev/debugger/debugger.hpp>