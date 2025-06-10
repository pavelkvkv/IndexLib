//================ include/IndexLib/IndexTypes.hpp =============================
#ifndef INDEXLIB_INDEXTYPES_HPP
#define INDEXLIB_INDEXTYPES_HPP

#include <cstdint>
#include <string>
#include "json.hpp"

// Отключаем исключения в nlohmann::json
#ifndef JSON_NOEXCEPTION
#define JSON_NOEXCEPTION 1
#endif

namespace IndexLib
{

	// Константы
	constexpr uint32_t kCacheSizeMax = 8;	  //!< Максимум объектов в кеше
	constexpr uint32_t kCacheLifeMs	 = 10000; //!< 10 секунд бездействия
	constexpr uint32_t kTaskQueueLen = 16;	  //!< Глубина очереди задач
	constexpr uint32_t kTaskStack	 = 4096;  //!< Стек тасков FreeRTOS
	constexpr uint32_t kTaskPrio	 = 3;	  //!< Приоритет тасков

// Формат индексного файла: имя index, содержит MessagePack с таким содержимым (пример одной записи):
// {
//   "idx": 12,           // порядковый номер в индексном файле (index)
//   "sz": 123456,        // размер файла (size)
//   "t": 1,              // тип файла (type)
//   "arc": false,        /* archived — файл из rec скопирован в arc, означает, что файл есть и в rec/file.wav, и в arc/file.wav. Ставится/удаляется в обоих папках. */
//   "del": false,        // deleted — запись не связана с файлом
//   "fn": "record.wav",  // filename — только имя файла без пути

//   "aud": {             // audio — вложенная структура аудио-информации
//     "ch": 2,           // channel — номер канала
//     "yy": 25,          // год (относительно 2000)
//     "MM": 6,           // месяц
//     "dd": 9,           // день
//     "hh": 14,          // часы
//     "mm": 30,          // минуты
//     "ss": 45,          // секунды
//     "cdc": "",           // codec
//     "dur": 1200,       // duration — длительность в секундах
//     "audr": true,        // audiorecord — флаг аудиозаписи
//     "radr": false,       // radiorecord — флаг радиозаписи
//     "opo": true,        // оповещение
//     "opopl": false       // opo_playlist
//   }
// }

	using Json = nlohmann::json;

} // namespace IndexLib

#endif // INDEXLIB_INDEXTYPES_HPP