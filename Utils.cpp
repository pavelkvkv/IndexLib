//================ src/Utils.cpp ==============================================
#include "Utils.hpp"
#include "Fapi.hpp"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cstdio>
#include <cstring>

#define TAG "IndexUtils"

namespace IndexLib
{

int extractFileMeta(const std::string &path,
                    uint32_t &size,
                    uint32_t &duration,
                    uint8_t &type,
                    std::string &codec)
	{
		auto &fs = Fapi::instance();
		size	 = 0;
		duration = 0;
		type     = 0;
		codec.clear();
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
                        if (size >= 22)
                        {
                                uint16_t fmt = buf[20] | (buf[21] << 8);
                                codec        = std::to_string(fmt);
                        }
                        else
                        {
                                codec = "wav";
                        }
                }
                else if (size >= 3 && buf[0] == 'I' && buf[1] == 'D' && buf[2] == '3')
                {
                        type  = 2; // MP3
                        codec = "mp3";
                        // duration оценить сложно без парсинга – оставим 0
                }
                else
                {
                        type = 0; // unknown
                        auto pos = path.find_last_of('.');
                        if (pos != std::string::npos) codec = path.substr(pos + 1);
                }
                return 0;
        }

        bool parseAudFromFilename(const std::string &filename,
                                 uint8_t &ch,
                                 uint16_t &yy,
                                 uint8_t &MM,
                                 uint8_t &dd,
                                 uint8_t &hh,
                                 uint8_t &mm,
                                 uint8_t &ss)
        {
                std::string name = filename;
                auto dot           = name.find_last_of('.');
                if (dot != std::string::npos) name = name.substr(0, dot);
                int chInt, y, mo, d, h, mi, se;
                if (std::sscanf(name.c_str(), "audio%u %d-%d-%d %d:%d:%d",
                               &chInt, &y, &mo, &d, &h, &mi, &se) == 7)
                {
                        ch = static_cast<uint8_t>(chInt);
                        yy = static_cast<uint16_t>(y - 2000);
                        MM = static_cast<uint8_t>(mo);
                        dd = static_cast<uint8_t>(d);
                        hh = static_cast<uint8_t>(h);
                        mm = static_cast<uint8_t>(mi);
                        ss = static_cast<uint8_t>(se);
                        return true;
                }
                ch = 0;
                yy = MM = dd = hh = mm = ss = 0;
                return false;
        }

        uint32_t tickMs()
        {
                return static_cast<uint32_t>(xTaskGetTickCount() * portTICK_PERIOD_MS);
        }

} // namespace IndexLib
