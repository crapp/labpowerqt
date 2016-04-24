// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015, 2016 Christian Rapp <0x2a at posteo dot org>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
