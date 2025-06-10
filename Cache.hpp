//================ include/IndexLib/Cache.hpp ================================
#ifndef INDEXLIB_CACHE_HPP
#define INDEXLIB_CACHE_HPP

#include "IndexTypes.hpp"
#include "Parser.hpp"
#include <vector>
#include <mutex>

namespace IndexLib {

struct CacheEntry {
    std::string dir; //!< каталог, к которому относится индекс
    Json         json;
    uint32_t     lastUseMs{0};
    bool         dirty{false};
};

//! Кеширование индексных объектов.
class Cache {
public:
    static Cache &instance();

    //! Получить json индекса каталога (загрузит при необходимости).
    int acquire(const std::string &dir, Json *&outJson);

    //! Отметить json как изменённый.
    void setDirty(Json *json);

    //! Сбросить один кеш или все.
    void drop(const std::string &dir);
    void dropAll();

    //! Синхронно сохранить все dirty кеши.
    void flushAll();

private:
    Cache(); // создаёт таск
    static void cacheTask(void *arg);
    void taskLoop();

    //! Внутренние.
    int  loadToCache(const std::string &dir, Json &j);
    int  saveFromCache(CacheEntry &e);
    CacheEntry *find(const std::string &dir);
    void evictIfNeeded();

    std::vector<CacheEntry> _entries;
    std::mutex              _mtx; //!< mutex std, тк atomics нет
};

} // namespace IndexLib
#endif // INDEXLIB_CACHE_HPP