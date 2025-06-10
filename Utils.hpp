//================ include/IndexLib/Utils.hpp =================================
#ifndef INDEXLIB_UTILS_HPP
#define INDEXLIB_UTILS_HPP

#include <string>
#include <vector>
#include "IndexTypes.hpp"

namespace IndexLib
{

        //! Извлечь метаданные файла.
        int extractFileMeta(const std::string &path,
                            uint32_t &size,
                            uint32_t &duration,
                            uint8_t &type,
                            std::string &codec);

        //! Распарсить дату и номер канала из имени файла.
        bool parseAudFromFilename(const std::string &filename,
                                 uint8_t &ch,
                                 uint16_t &yy,
                                 uint8_t &MM,
                                 uint8_t &dd,
                                 uint8_t &hh,
                                 uint8_t &mm,
                                 uint8_t &ss);

	//! Получить текущее время в мс от запуска.
	uint32_t tickMs();

} // namespace IndexLib

#endif // INDEXLIB_UTILS_HPP

