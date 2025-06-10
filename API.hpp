//================ include/IndexLib/API.hpp ==================================
#ifndef INDEXLIB_API_HPP
#define INDEXLIB_API_HPP

#include <string>
#include "IndexTypes.hpp"
#include "TaskQueue.hpp"

namespace IndexLib
{

	class API
	{
	public:
		//! Получить размер каталога или файла из индекса.
		static int getSize(const std::string &path, uint32_t &size);

		//! Получить суммарную длительность.
		static int getDuration(const std::string &path, uint32_t &dur);

		//! Получить весь json-запись о файле.
		static int getRecord(const std::string &filePath, Json &record);

		//! Добавить задачу (не ждать).
		static int scheduleTask(TaskType type, const std::string &path);

		//! Добавить задачу и ждать (front=true).
		static int scheduleTaskSync(TaskType type, const std::string &path);
	};

} // namespace IndexLib

#endif // INDEXLIB_API_HPP