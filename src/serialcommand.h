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

#ifndef SERIALCOMMAND
#define SERIALCOMMAND

#include <QByteArray>
#include <QVariant>
#include <memory>

struct SerialCommand {
public:
    /**
     * @brief SerialCommand default constructor that we need to register our
     * custom Type
     */
    SerialCommand()
    {
        this->command = 100;
        this->powerSupplyChannel = 1;
        this->value = QVariant();
        this->commandWithReply = false;
    };

    SerialCommand(int command, int channel = 1, QByteArray reply = QByteArray(),
                  bool withReply = false)
        : command(command), powerSupplyChannel(channel), reply(reply),
          commandWithReply(withReply){};

    int getCommand() { return this->command; };
    int getPowerSupplyChannel() { return this->powerSupplyChannel; };
    QVariant getValue() { return this->value; }
    bool getCommandWithReply() { return this->commandWithReply; };

    void setReply(QByteArray data)
    {
        this->reply = data;
        this->value = this->reply;
    };
    QByteArray getReply() { return this->reply; }

private:
    int command;
    int powerSupplyChannel;
    QVariant value;
    bool commandWithReply;

    QByteArray reply;
};

// Register our metatype. Needed to send this kind of object wrapped in a std
// smart pointer via SIGNAL/SLOT mechanism
Q_DECLARE_METATYPE(std::shared_ptr<SerialCommand>)

#endif // SERIALCOMMAND
