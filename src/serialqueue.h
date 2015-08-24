// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015 Christian Rapp <0x2a at posteo dot org>

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

#ifndef SERIALQUEUE_H
#define SERIALQUEUE_H

#include <mutex>
#include <memory>
/*
 * We use a std::queue as basis for this threadsafe queue
 */
#include <queue>
/*
 * We need a conditional variable to signal a waiting thread
 */
#include <condition_variable>

#include "serialcommand.h"

class SerialQueue
{
public:
    SerialQueue();

    void push(int command, int channel = 1, QVariant value = QVariant(),
              bool withReply = false);
    std::shared_ptr<SerialCommand> pop();

    bool empty();

private:
    std::queue<std::shared_ptr<SerialCommand>> internalQueue;
    std::mutex mtx;
    std::condition_variable threadWakeUpCondition;
};

#endif // SERIALQUEUE_H
