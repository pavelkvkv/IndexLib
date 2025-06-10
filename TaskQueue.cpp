//================ src/TaskQueue.cpp =========================================
#include "TaskQueue.hpp"
#include "Builder.hpp"
#include "log.h"
#include <cstring>
#include <memory>

#define TAG "TaskQueue"

namespace IndexLib {

static TaskQueue g_q;

TaskQueue &TaskQueue::instance() { return g_q; }

TaskQueue::TaskQueue() {
    _q = xQueueCreate(kTaskQueueLen, sizeof(Task*));
    xTaskCreate(taskFunc, TAG, kTaskStack, this, kTaskPrio, nullptr);
}

void TaskQueue::taskFunc(void *arg) {
    static_cast<TaskQueue*>(arg)->process();
}

void TaskQueue::process() {
    while (true) {
        Task *ptask = nullptr;
        if (xQueueReceive(_q, &ptask, portMAX_DELAY) == pdTRUE && ptask) {
            std::unique_ptr<Task> uptr(ptask);
            Task t = *uptr;
            switch (t.type) {
                case TaskType::UPDATE_FILE:
                    Builder::updateFile(t.path, false);
                    break;
                case TaskType::RESCAN_DIR:
                    Builder::scanDirectory(t.path);
                    break;
                case TaskType::ARC_FLAG:
                    Builder::setArc(t.path, true);
                    break;
            }
        }
    }
}

int TaskQueue::enqueue(TaskType type, const std::string &path, bool front) {
    std::unique_ptr<Task> t = std::make_unique<Task>(Task{type, path});
    if (!t) return -1;
    Task *raw = t.release();
    BaseType_t rc;
    if (front)
        rc = xQueueSendToFront(_q, &raw, portMAX_DELAY);
    else
        rc = xQueueSendToBack(_q, &raw, portMAX_DELAY);
    if (rc != pdTRUE) {
        delete raw;
        return -2;
    }
    return 0;
}

} // namespace IndexLib
