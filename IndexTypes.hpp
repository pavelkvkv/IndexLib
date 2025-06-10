//================ include/IndexLib/IndexTypes.hpp =============================
#ifndef INDEXLIB_INDEXTYPES_HPP
#define INDEXLIB_INDEXTYPES_HPP

#include <cstdint>
#include <string>
#include "json.hpp"

// Отключаем исключения в nlohmann::json
#ifndef JSON_NOEXCEPTION
#define JSON_NOEXCEPTION 1
#endif

namespace IndexLib {

// Константы
constexpr uint32_t kCacheSizeMax   = 8;       //!< Максимум объектов в кеше
constexpr uint32_t kCacheLifeMs    = 10000;   //!< 10 секунд бездействия
constexpr uint32_t kTaskQueueLen   = 16;      //!< Глубина очереди задач
constexpr uint32_t kTaskStack      = 4096;    //!< Стек тасков FreeRTOS
constexpr uint32_t kTaskPrio       = 3;       //!< Приоритет тасков

//------------------------------------------------------------------------------
// Структуры индекса
//------------------------------------------------------------------------------
struct AudioInfo {
    uint8_t ch{};
    uint8_t yy{};  //!< год (от 2000)
    uint8_t MM{};
    uint8_t dd{};
    uint8_t hh{};
    uint8_t mm{};
    uint8_t ss{};
    char    cdc[8]{}; //!< codec (строка макс 7 + 0)
    uint32_t dur{};   //!< длительность (сек)
    bool audr{};
    bool radr{};
    bool opo{};
    bool opopl{};
};

struct IndexRecord {
    uint32_t idx{};
    uint32_t sz{};
    uint8_t  t{};
    bool     arc{};
    bool     del{};
    char     fn[64]{};   //!< filename
    AudioInfo aud;
};

using Json = nlohmann::json;

} // namespace IndexLib

#endif // INDEXLIB_INDEXTYPES_HPP