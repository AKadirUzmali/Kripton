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

// Include:
#include <developer/config/Build.h>
#include <developer/config/Config.h>
#include <developer/config/Version.h>

#include <developer/core/Source.h>

#include <developer/log/Levels.h>
#include <developer/log/Logger.h>
#include <developer/log/Macros.h>

#include <developer/trace/Timeline.h>
#include <developer/trace/Trace.h>

#include <developer/test/Expect.h>
#include <developer/test/Assert.h>

#include <kits/ToolKit.h>
#include <kits/OutputKit.h>