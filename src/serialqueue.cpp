#include "serialqueue.h"

SerialQueue::SerialQueue() {}

void SerialQueue::push(int command, int channel, const QVariant &value,
                       bool withReply, int replyLength)
{
    QMutexLocker qlock(&this->qmtx);
    std::shared_ptr<SerialCommand> com = std::make_shared<SerialCommand>(
        command, channel, value, withReply, replyLength);

    this->internalQueue.push(com);
    // notify thread to wake up and pop latest command
    this->qcondition.wakeOne();
}

std::shared_ptr<SerialCommand> SerialQueue::pop()
{
    QMutexLocker qlock(&this->qmtx);

    // this unlocks our mutex and waits until our internal queue
    // is no longer empty.
    if (this->internalQueue.empty())
        this->qcondition.wait(&this->qmtx);

    std::shared_ptr<SerialCommand> com = this->internalQueue.front();
    this->internalQueue.pop();

    return com;
}

bool SerialQueue::empty()
{
    QMutexLocker qlock(&this->qmtx);
    return this->internalQueue.empty();
}
