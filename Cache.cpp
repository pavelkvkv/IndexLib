//================ src/Cache.cpp =============================================
#include "Cache.hpp"
#include "Utils.hpp"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"

#define TAG "Cache"

namespace IndexLib {

static Cache g_cache;

Cache &Cache::instance() { return g_cache; }

Cache::Cache() {
    xTaskCreate(cacheTask, TAG, kTaskStack, this, kTaskPrio, nullptr);
}

void Cache::cacheTask(void *arg) {
    static_cast<Cache*>(arg)->taskLoop();
}

void Cache::taskLoop() {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        uint32_t now = tickMs();
        LockGuard lk(_mtx);
        for (auto it = _entries.begin(); it != _entries.end();) {
            if ((now - it->lastUseMs) > kCacheLifeMs) {
                if (it->dirty) {
                    saveFromCache(*it);
                }
                logI("evict %s", it->dir.c_str());
                it = _entries.erase(it);
            } else {
                ++it;
            }
        }
    }
}

CacheEntry *Cache::find(const std::string &dir) {
    for (auto &e : _entries) {
        if (e.dir == dir) return &e;
    }
    return nullptr;
}

int Cache::loadToCache(const std::string &dir, Json &j) {
    std::string path = dir + "/index";
    int rc = Parser::load(path, j);
    if (rc < 0) {
        // если файл не найден – создаём пустой Json
        j = Json::object();
    }
    return 0;
}

int Cache::saveFromCache(CacheEntry &e) {
    std::string path = e.dir + "/index";
    int rc = Parser::save(path, e.json);
    if (rc == 0) e.dirty = false;
    return rc;
}

void Cache::evictIfNeeded() {
    if (_entries.size() < kCacheSizeMax) return;
    // найти самый старый
    size_t idxOld = 0;
    uint32_t oldest = _entries[0].lastUseMs;
    for (size_t i=1;i<_entries.size();++i) {
        if (_entries[i].lastUseMs < oldest) {
            oldest = _entries[i].lastUseMs;
            idxOld = i;
        }
    }
    if (_entries[idxOld].dirty) saveFromCache(_entries[idxOld]);
    _entries.erase(_entries.begin()+idxOld);
}

int Cache::acquire(const std::string &dir, Json *&outJson) {
    LockGuard lk(_mtx);
    CacheEntry *e = find(dir);
    if (!e) {
        // загрузка
        CacheEntry ne;
        ne.dir = dir;
        if (loadToCache(dir, ne.json) < 0) return -1;
        ne.lastUseMs = tickMs();
        ne.dirty = false;
        evictIfNeeded();
        _entries.push_back(std::move(ne));
        e = &_entries.back();
    }
    e->lastUseMs = tickMs();
    outJson = &e->json;
    return 0;
}

void Cache::setDirty(Json *json) {
    LockGuard lk(_mtx);
    for (auto &e : _entries) {
        if (&e.json == json) {
            e.dirty = true;
            break;
        }
    }
}

void Cache::drop(const std::string &dir) {
    LockGuard lk(_mtx);
    for (auto it=_entries.begin(); it!=_entries.end(); ++it) {
        if (it->dir == dir) {
            if (it->dirty) saveFromCache(*it);
            _entries.erase(it);
            break;
        }
    }
}

void Cache::dropAll() {
    LockGuard lk(_mtx);
    for (auto &e : _entries) {
        if (e.dirty) saveFromCache(e);
    }
    _entries.clear();
}

void Cache::flushAll() {
    LockGuard lk(_mtx);
    for (auto &e : _entries) {
        if (e.dirty) saveFromCache(e);
    }
}

} // namespace IndexLib