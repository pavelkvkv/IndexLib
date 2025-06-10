//================ include/IndexLib/Utils.hpp =================================
#ifndef INDEXLIB_UTILS_HPP
#define INDEXLIB_UTILS_HPP

#include <string>
#include <vector>
#include "IndexTypes.hpp"

namespace IndexLib
{

	//! Извлечь метаданные файла (размер, длительность, тип).
	int extractFileMeta(const std::string &path, uint32_t &size, uint32_t &duration, uint8_t &type);

	//! Получить текущее время в мс от запуска.
	uint32_t tickMs();

} // namespace IndexLib

#endif // INDEXLIB_UTILS_HPP