// Abdulkadir U. - 11/01/2026
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
#include <cstddef>

#include <devel/config/DevelBuild.h>
#include <devel/config/DevelFeatures.h>
#include <devel/config/DevelVersion.h>

#include <devel/core/Category.h>
#include <devel/core/Contract.h>
#include <devel/core/Event.h>
#include <devel/core/Source.h>

#include <devel/log/Levels.h>
#include <devel/log/Sinks.h>
#include <devel/log/Logger.h>
#include <devel/log/Macros.h>

#include <devel/trace/Timeline.h>
#include <devel/trace/Trace.h>

#include <devel/test/Expect.h>
#include <devel/test/Assert.h>