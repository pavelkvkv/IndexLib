//================ src/Utils.cpp ==============================================
#include "Utils.hpp"
#include "Fapi.hpp"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"

#define TAG "IndexUtils"

namespace IndexLib
{

	int extractFileMeta(const std::string &path, uint32_t &size, uint32_t &duration, uint8_t &type)
	{
		auto &fs = Fapi::instance();
		size	 = 0;
		duration = 0;
		type	 = 0;
		std::vector<uint8_t> buf;
		if (fs.readFile(path.c_str(), buf) < 0)
		{
			logE("extract meta failed: %s", path.c_str());
			return -1;
		}
		size = static_cast<uint32_t>(buf.size());

		// Простая эвристика: WAV? ("RIFF" + "WAVE"), MP3? (ID3 header)
		if (size >= 12 && buf[0] == 'R' && buf[1] == 'I' && buf[2] == 'F' && buf[8] == 'W' && buf[9] == 'A')
		{
			type = 1; // WAV
			if (size >= 44)
			{
				uint32_t bytesPerSec = buf[28] | (buf[29] << 8) | (buf[30] << 16) | (buf[31] << 24);
				if (bytesPerSec) duration = size / bytesPerSec;
			}
		}
		else if (size >= 3 && buf[0] == 'I' && buf[1] == 'D' && buf[2] == '3')
		{
			type = 2; // MP3
			// duration оценить сложно без парсинга – оставим 0
		}
		else
		{
			type = 0; // unknown
		}
		return 0;
	}

	uint32_t tickMs()
	{
		return static_cast<uint32_t>(xTaskGetTickCount() * portTICK_PERIOD_MS);
	}

} // namespace IndexLib