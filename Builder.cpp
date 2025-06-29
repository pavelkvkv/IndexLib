//================ src/Builder.cpp ===========================================
#include "Builder.hpp"
#include "Cache.hpp"
#include "Utils.hpp"
#include "Fapi.hpp"
#include "log.h"
#include <cstring>
#include <vector>
#include <algorithm>

#define TAG "Builder"

namespace IndexLib
{

	static void updateParentSizes(const std::string &dir, int32_t deltaSize, int32_t deltaDur)
	{
		std::string cur = dir;
		while (!cur.empty())
		{
			Json *j;
			if (Cache::instance().acquire(cur, j) == 0)
			{
				(*j)["size"]	 = (*j).value("size", 0) + deltaSize;
				(*j)["duration"] = (*j).value("duration", 0) + deltaDur;
				Cache::instance().setDirty(j);
			}
			// поднимаемся на уровень выше
			auto pos = cur.find_last_of('/');
			if (pos == std::string::npos) break;
			cur = cur.substr(0, pos);
		}
	}

	int Builder::scanDirectory(const std::string &dir)
	{
		auto &fs = Fapi::instance();
		auto dh	 = fs.openDirectory(dir.c_str());
		if (dh < 0)
		{
			logE("openDirectory failed: %s", dir.c_str());
			return -1;
		}
		// Для простоты пройдём все файлы с суффиксами .wav/.mp3
		std::vector<std::string> files; /* Fapi не предоставляет enum – предполагаем внешнюю */
		// TODO: реализация обхода – платформенно-зависима, опущено.

		Json *root;
		Cache::instance().acquire(dir, root);

		for (auto &f : files)
		{
			std::string full = dir + "/" + f;
                        uint32_t sz, dur;
                        uint8_t type;
                        std::string codec;
                        if (extractFileMeta(full, sz, dur, type, codec) == 0)
                        {
                                uint8_t ch, MM, dd, hh, mm, ss; uint16_t yy;
                                parseAudFromFilename(f, ch, yy, MM, dd, hh, mm, ss);
                                bool arcFlag = false;
                                auto posRec = full.find("/rec/");
                                auto posArc = full.find("/arc/");
                                if (posArc != std::string::npos)
                                        arcFlag = true;
                                else if (posRec != std::string::npos)
                                {
                                        std::string other = full;
                                        other.replace(posRec, 5, "/arc/");
                                        if (fs.fileExists(other.c_str())) arcFlag = true;
                                }
                                Json rec = {
                                        {"idx", (*root).size()},
                                        {"sz", sz},
                                        {"t", type},
                                        {"arc", arcFlag},
                                        {"del", false},
                                        {"fn", f},
                                };
                                rec["aud"] = {
                                        {"dur", dur},
                                        {"ch", ch},
                                        {"yy", yy},
                                        {"MM", MM},
                                        {"dd", dd},
                                        {"hh", hh},
                                        {"mm", mm},
                                        {"ss", ss},
                                        {"cdc", codec},
                                        {"audr", true},
                                        {"radr", false},
                                        {"opo", false},
                                        {"opopl", false}};
				(*root).push_back(rec);
				Cache::instance().setDirty(root);
				// обновить размеры каталога и выше
				updateParentSizes(dir, sz, dur);
			}
		}
		return 0;
	}

	int Builder::updateFile(const std::string &filePath, bool remove)
	{
		// Разделим path на dir и filename
		auto pos = filePath.find_last_of('/');
		if (pos == std::string::npos) return -1;
		std::string dir = filePath.substr(0, pos);
		std::string fn	= filePath.substr(pos + 1);

		Json *j;
		if (Cache::instance().acquire(dir, j) < 0) return -2;

		// найти запись
		int32_t deltaSz = 0, deltaDur = 0;
		for (auto &rec : *j)
		{
			if (rec["fn"].get<std::string>() == fn)
			{
				uint32_t oldSz	= rec["sz"].get<uint32_t>();
				uint32_t oldDur = rec["aud"]["dur"].get<uint32_t>();
				if (remove)
				{
					rec["del"] = true;
					deltaSz -= oldSz;
					deltaDur -= oldDur;
				}
                                else
                                {
                                        // освежаем метаданные
                                        std::string codec;
                                        uint32_t sz, dur;
                                        uint8_t type;
                                        extractFileMeta(filePath, sz, dur, type, codec);
                                        rec["sz"]          = sz;
                                        rec["t"]           = type;
                                        rec["aud"]["dur"] = dur;
                                        rec["aud"]["cdc"] = codec;
                                        uint8_t ch, MM, dd, hh, mm, ss; uint16_t yy;
                                        parseAudFromFilename(fn, ch, yy, MM, dd, hh, mm, ss);
                                        rec["aud"]["ch"] = ch;
                                        rec["aud"]["yy"] = yy;
                                        rec["aud"]["MM"] = MM;
                                        rec["aud"]["dd"] = dd;
                                        rec["aud"]["hh"] = hh;
                                        rec["aud"]["mm"] = mm;
                                        rec["aud"]["ss"] = ss;
                                        bool arcFlag = false;
                                        auto posRec  = filePath.find("/rec/");
                                        auto posArc  = filePath.find("/arc/");
                                        if (posArc != std::string::npos)
                                                arcFlag = true;
                                        else if (posRec != std::string::npos)
                                        {
                                                std::string other = filePath;
                                                other.replace(posRec, 5, "/arc/");
                                                if (Fapi::instance().fileExists(other.c_str())) arcFlag = true;
                                        }
                                        rec["arc"]          = arcFlag;
                                        deltaSz += (int32_t)sz - (int32_t)oldSz;
                                        deltaDur += (int32_t)dur - (int32_t)oldDur;
                                }
				Cache::instance().setDirty(j);
				break;
			}
		}
		updateParentSizes(dir, deltaSz, deltaDur);
		return 0;
	}

	int Builder::setArc(const std::string &filePath, bool on)
	{
		// Устанавливаем метку arc в обеих папках rec и arc
		// Определяем pair pathRec / pathArc
		std::string pathRec = filePath;
		std::string pathArc = filePath;
		if (pathRec.find("/rec/") != std::string::npos)
		{
			pathArc.replace(pathRec.find("/rec/"), 4, "/arc/");
		}
		else if (pathRec.find("/arc/") != std::string::npos)
		{
			pathRec.replace(pathRec.find("/arc/"), 4, "/rec/");
		}
		auto updateMark = [&](const std::string &p)
		{
        auto pos = p.find_last_of('/');
        if (pos==std::string::npos) return;
        std::string dir = p.substr(0,pos);
        std::string fn  = p.substr(pos+1);
        Json *j; if (Cache::instance().acquire(dir,j)<0) return;
        for (auto &rec:*j) if (rec["fn"].get<std::string>()==fn) {rec["arc"]=on; Cache::instance().setDirty(j);} };
		updateMark(pathRec);
		updateMark(pathArc);
		return 0;
	}

} // namespace IndexLib