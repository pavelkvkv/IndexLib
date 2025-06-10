//================ include/IndexLib/Builder.hpp ===============================
#ifndef INDEXLIB_BUILDER_HPP
#define INDEXLIB_BUILDER_HPP

#include "IndexTypes.hpp"
#include <string>

namespace IndexLib {

class Builder {
public:
    //! Рекурсивное сканирование папки и создание/обновление индекса.
    static int scanDirectory(const std::string &dir);

    //! Обновить запись о файле (удалить/добавить/изменить).
    static int updateFile(const std::string &filePath, bool remove = false);

    //! Установить или снять флаг ARC.
    static int setArc(const std::string &filePath, bool on);
};

} // namespace IndexLib

#endif // INDEXLIB_BUILDER_HPP