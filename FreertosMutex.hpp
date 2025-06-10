#ifndef INDEXLIB_FREERTOS_MUTEX_HPP
#define INDEXLIB_FREERTOS_MUTEX_HPP

#include "FreeRTOS.h"
#include "semphr.h"

namespace IndexLib
{

	class FreertosMutex
	{
	public:
		FreertosMutex() { _handle = xSemaphoreCreateMutex(); }
		~FreertosMutex()
		{
			if (_handle) vSemaphoreDelete(_handle);
		}
		void lock() { xSemaphoreTake(_handle, portMAX_DELAY); }
		void unlock() { xSemaphoreGive(_handle); }

		FreertosMutex(const FreertosMutex &)			= delete;
		FreertosMutex &operator=(const FreertosMutex &) = delete;

	private:
		SemaphoreHandle_t _handle;
	};

	class LockGuard
	{
	public:
		explicit LockGuard(FreertosMutex &m) : _m(m) { _m.lock(); }
		~LockGuard() { _m.unlock(); }

		LockGuard(const LockGuard &)			= delete;
		LockGuard &operator=(const LockGuard &) = delete;

	private:
		FreertosMutex &_m;
	};

} // namespace IndexLib

#endif // INDEXLIB_FREERTOS_MUTEX_HPP
