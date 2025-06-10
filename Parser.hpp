//================ include/IndexLib/Parser.hpp ================================
#ifndef INDEXLIB_PARSER_HPP
#define INDEXLIB_PARSER_HPP

#include "IndexTypes.hpp"
#include <string>

namespace IndexLib {

class Parser {
public:
    //! Загрузить индексный файл в Json.
    static int load(const std::string &path, Json &outJson);

    //! Сохранить Json в индексный файл.
    static int save(const std::string &path, const Json &j);
};

} // namespace IndexLib

#endif // INDEXLIB_PARSER_HPP