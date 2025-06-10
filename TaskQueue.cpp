//================ src/TaskQueue.cpp =========================================
#include "TaskQueue.hpp"
#include "Builder.hpp"
#include "log.h"
#include <cstring>

#define TAG "TaskQueue"

namespace IndexLib {

static TaskQueue g_q;

TaskQueue &TaskQueue::instance() { return g_q; }

TaskQueue::TaskQueue() {
    _q = xQueueCreate(kTaskQueueLen, sizeof(Task*));
    xTaskCreate(taskFunc, TAG, kTaskStack, this, kTaskPrio, nullptr);
}

void TaskQueue::taskFunc(void *arg) {
    static_cast<TaskQueue*>(arg)->process(*reinterpret_cast<Task*>(nullptr));
}

void TaskQueue::process(Task &dummy) {
    (void) dummy; // unused
    while (true) {
        Task *ptask = nullptr;
        if (xQueueReceive(_q, &ptask, portMAX_DELAY) == pdTRUE && ptask) {
            Task t = *ptask;
            delete ptask;
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
    Task *t = new Task{type, path};
    if (!t) return -1;
    BaseType_t rc;
    if (front)
        rc = xQueueSendToFront(_q, &t, portMAX_DELAY);
    else
        rc = xQueueSendToBack(_q, &t, portMAX_DELAY);
    return (rc == pdTRUE) ? 0 : -2;
}

} // namespace IndexLib