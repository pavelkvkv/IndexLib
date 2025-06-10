//================ src/Parser.cpp ============================================
#include "Parser.hpp"
#include "Fapi.hpp"
#include "log.h"

#define TAG "Parser"

namespace IndexLib {

int Parser::load(const std::string &path, Json &outJson) {
    auto &fs = Fapi::instance();
    std::vector<uint8_t> buf;
    if (fs.readFile(path.c_str(), buf) < 0) {
        logE("readFile failed: %s", path.c_str());
        return -1;
    }
    auto j = Json::from_msgpack(buf, false, false);
    if (j.is_discarded()) {
        logE("msgpack parse error: %s", path.c_str());
        return -2;
    }
    outJson = j;
    return 0;
}

int Parser::save(const std::string &path, const Json &j) {
    auto &fs = Fapi::instance();
    std::vector<uint8_t> buf = Json::to_msgpack(j);
    if (fs.writeFile(path.c_str(), buf) < 0) {
        logE("writeFile failed: %s", path.c_str());
        return -1;
    }
    return 0;
}

} // namespace IndexLib