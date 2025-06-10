//================ src/API.cpp ===============================================
#include "API.hpp"
#include "Cache.hpp"
#include "Builder.hpp"
#include "log.h"
#include <cstring>

#define TAG "API"

namespace IndexLib
{

	static int ensureIndexReady(const std::string &dir)
	{
		Json *j;
		if (Cache::instance().acquire(dir, j) == 0) return 0;
		// если нет – создаём задачу сканирования и ждём
		TaskQueue::instance().enqueue(TaskType::RESCAN_DIR, dir, true);
		// простое ожидание – polling (можно условную переменную, но без atomic)
		for (int i = 0; i < 50; ++i)
		{ // 5 секунд
			vTaskDelay(pdMS_TO_TICKS(100));
			if (Cache::instance().acquire(dir, j) == 0) return 0;
		}
		return -1;
	}

	static bool isDirectory(const std::string &path)
	{
		return path.back() == '/';
	}

	int API::getSize(const std::string &path, uint32_t &size)
	{
		if (isDirectory(path))
		{
			Json *j;
			if (ensureIndexReady(path) < 0 || Cache::instance().acquire(path, j) < 0) return -1;
			size = (*j).value("size", 0);
		}
		else
		{
			auto pos		= path.find_last_of('/');
			std::string dir = path.substr(0, pos);
			std::string fn	= path.substr(pos + 1);
			Json *j;
			if (ensureIndexReady(dir) < 0 || Cache::instance().acquire(dir, j) < 0) return -1;
			for (auto &rec : *j)
				if (rec["fn"].get<std::string>() == fn)
				{
					size = rec["sz"].get<uint32_t>();
					return 0;
				}
			return -2;
		}
		return 0;
	}

	int API::getDuration(const std::string &path, uint32_t &dur)
	{
		if (isDirectory(path))
		{
			Json *j;
			if (ensureIndexReady(path) < 0 || Cache::instance().acquire(path, j) < 0) return -1;
			dur = (*j).value("duration", 0);
		}
		else
		{
			auto pos		= path.find_last_of('/');
			std::string dir = path.substr(0, pos);
			std::string fn	= path.substr(pos + 1);
			Json *j;
			if (ensureIndexReady(dir) < 0 || Cache::instance().acquire(dir, j) < 0) return -1;
			for (auto &rec : *j)
				if (rec["fn"].get<std::string>() == fn)
				{
					dur = rec["aud"]["dur"].get<uint32_t>();
					return 0;
				}
			return -2;
		}
		return 0;
	}

	int API::getRecord(const std::string &filePath, Json &record)
	{
		auto pos = filePath.find_last_of('/');
		if (pos == std::string::npos) return -1;
		std::string dir = filePath.substr(0, pos);
		std::string fn	= filePath.substr(pos + 1);
		Json *j;
		if (ensureIndexReady(dir) < 0 || Cache::instance().acquire(dir, j) < 0) return -1;
		for (auto &rec : *j)
			if (rec["fn"].get<std::string>() == fn)
			{
				record = rec;
				return 0;
			}
		return -2;
	}

	int API::scheduleTask(TaskType type, const std::string &path)
	{
		return TaskQueue::instance().enqueue(type, path);
	}

	int API::scheduleTaskSync(TaskType type, const std::string &path)
	{
		return TaskQueue::instance().enqueue(type, path, true);
	}

} // namespace IndexLib