/*
 * This file is part of RaidFinder
 * Copyright (C) 2019 by Admiral_Fish
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PROFILE_HPP
#define PROFILE_HPP

#include <Core/Game.hpp>
#include <Core/Global.hpp>
#include <QString>

class Profile
{
public:
    Profile();
    Profile(const QString &name, u16 tid, u16 sid, Game version);
    QString getName() const;
    u16 getTID() const;
    u16 getSID() const;
    u16 getTSV() const;
    Game getVersion() const;
    QString getVersionString() const;

private:
    QString name;
    u16 tid;
    u16 sid;
    Game version;
};

bool operator==(const Profile &left, const Profile &right);
bool operator!=(const Profile &left, const Profile &right);

#endif // PROFILE_HPP
