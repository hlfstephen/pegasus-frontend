// Pegasus Frontend
// Copyright (C) 2017-2020  Mátyás Mustoha
// Modified by hlfstephen, 2025-06-15
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include "providers/Provider.h"
#include <QJsonObject>
#include <QString>


namespace providers {
namespace media {

class MediaProvider : public Provider {
    Q_OBJECT

public:
    explicit MediaProvider(QObject* parent = nullptr);

    Provider& run(SearchContext&) final;
};

private:
    QString m_cache_path;
    QJsonObject m_cache_data;

    void load_cache();
    void save_cache();
};

} // namespace media
} // namespace providers
