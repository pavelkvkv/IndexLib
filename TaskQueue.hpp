//================ include/IndexLib/TaskQueue.hpp ============================
#ifndef INDEXLIB_TASKQUEUE_HPP
#define INDEXLIB_TASKQUEUE_HPP

#include <string>
#include "FreeRTOS.h"
#include "queue.h"
#include "IndexTypes.hpp"

namespace IndexLib
{

	enum class TaskType : uint8_t
	{
		UPDATE_FILE = 0,
		RESCAN_DIR	= 1,
		ARC_FLAG	= 2,
	};

	struct Task
	{
		TaskType type;
		std::string path;
	};

	class TaskQueue
	{
	public:
		static TaskQueue &instance();

		//! Добавить задачу (не блокирует).
		int enqueue(TaskType type, const std::string &path, bool front = false);

	private:
		TaskQueue();
		static void taskFunc(void *arg);
		void process();

		QueueHandle_t _q;
	};

} // namespace IndexLib

#endif // INDEXLIB_TASKQUEUE_HPP