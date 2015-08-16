#include "serialqueue.h"

SerialQueue::SerialQueue() {}

void SerialQueue::push(int command, int channel, QVariant value, bool withReply)
{
    std::lock_guard<std::mutex> lock(this->mtx);
    std::shared_ptr<SerialCommand> com =
        std::make_shared<SerialCommand>(command, channel, value, withReply);

    this->internalQueue.push(com);
    // notify thread to wake up and pop latest command
    this->threadWakeUpCondition.notify_one();
}

std::shared_ptr<SerialCommand> SerialQueue::pop()
{
    // we use a unique lock as conditional variable requires one
    std::unique_lock<std::mutex> lock(this->mtx);

    // this unlocks our mutex and waits unitl our internal queue
    // is no longer empty.
    this->threadWakeUpCondition.wait(
        lock, [this]() { return !this->internalQueue.empty(); });

    std::shared_ptr<SerialCommand> com = this->internalQueue.front();
    this->internalQueue.pop();

    return com;
}

bool SerialQueue::empty()
{
    std::lock_guard<std::mutex> lock(this->mtx);
    return this->internalQueue.empty();
}
