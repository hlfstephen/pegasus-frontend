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


#include "MediaProvider.h"

#include "PegasusAssets.h"
#include "model/gaming/Assets.h"
#include "model/gaming/Game.h"
#include "model/gaming/GameFile.h"
#include "types/AssetType.h"
#include "providers/SearchContext.h"
#include "utils/PathTools.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QStringBuilder>
#include <QStringList>
#include <array>


namespace {
const QStringList& allowed_asset_exts(AssetType type)
{
    static const QStringList empty_list({});
    static const QStringList image_exts { "png", "jpg", "webp", "apng" };
    static const QStringList video_exts { "webm", "mp4", "avi" };
    static const QStringList audio_exts { "mp3", "ogg", "wav" };

    switch (type) {
        case AssetType::UNKNOWN:
            return empty_list;
        case AssetType::VIDEO:
            return video_exts;
        case AssetType::MUSIC:
            return audio_exts;
        default:
            return image_exts;
    }
}

AssetType detect_asset_type(const QString& basename, const QString& ext)
{
    const AssetType type = pegasus_assets::str_to_type(basename);
    if (allowed_asset_exts(type).contains(ext))
        return type;

    return AssetType::UNKNOWN;
}

HashMap<QString, model::Game*> create_lookup_map(const HashMap<QString, model::GameFile*>& games)
{
    HashMap<QString, model::Game*> out;

    // TODO: C++17
    for (const auto& pair : games) {
        const QFileInfo fi(pair.first);
        model::Game* const game_ptr = pair.second->parentGame();

        QString extless_path = ::clean_abs_dir(fi) % QChar('/') % fi.completeBaseName();
        out.emplace(std::move(extless_path), game_ptr);

        // NOTE: the files are not necessarily in the same directory
        QString title_path = ::clean_abs_dir(fi) % QChar('/') % game_ptr->title();
        out.emplace(std::move(title_path), game_ptr);
    }

    return out;
}
} // namespace


namespace providers {
namespace media {

MediaProvider::MediaProvider(QObject* parent)
    : Provider(QLatin1String("pegasus_media"), QStringLiteral("Pegasus Media"), parent)
{
    QString frontendDir = paths::writableConfigDir();  // pegasus-frontend目录
    m_cache_path = frontendDir + "/media_cache.json";
}

Provider& MediaProvider::run(SearchContext& sctx)
{
    constexpr auto dir_filters = QDir::Files | QDir::NoDotAndDotDot;
    constexpr auto dir_flags = QDirIterator::Subdirectories | QDirIterator::FollowSymlinks;
    /*const std::array<QLatin1String, 2> MEDIA_SUBDIRS {
        QLatin1String("/media"),
        QLatin1String("/.media"),
    };*/
    const std::array<QLatin1String, 1> MEDIA_SUBDIRS {
        QLatin1String("/media"),
    };

    // 步骤1：加载缓存
    load_cache();

    // 步骤2：创建游戏查找映射（与原逻辑一致）
    const HashMap<QString, model::Game*> lookup_map = create_lookup_map(sctx.current_filepath_to_entry_map());

    // 步骤3：遍历媒体目录（优先使用缓存，仅扫描变更文件）
    QJsonArray cached_media_dirs = m_cache_data["media_dirs"].toArray();
    for (const QString& dir_base : sctx.pegasus_game_dirs()) {
        for (const QLatin1String& media_subdir_name : MEDIA_SUBDIRS) {
            const QString media_dir = dir_base % media_subdir_name;
            QDir dir(media_dir);
            if (!dir.exists()) continue;

            // 查找当前媒体目录的缓存记录（若存在）
            bool dir_exists_in_cache = false;
            QJsonObject cached_dir;
            for (const auto& item : cached_media_dirs) {
                QJsonObject obj = item.toObject();
                if (obj["dir_path"].toString() == media_dir) {
                    cached_dir = obj;
                    dir_exists_in_cache = true;
                    break;
                }
            }

            // 步骤3.1：使用缓存中的有效文件
            if (dir_exists_in_cache){
                QJsonArray cached_files = cached_dir["files"].toArray();
                for (const auto& item : cached_files) {
                    QJsonObject cached_file = item.toObject();
                    QString file_path = cached_file["file_path"].toString();
                    //QFileInfo fileinfo(file_path);

                    // 检查文件是否存在且未变更
                    //if (!fileinfo.exists() || !is_file_unchanged(fileinfo, cached_file))
                    //    continue;

                    // 查找关联的游戏（与原逻辑一致）
                    const QString lookup_key = cached_file["game_key"].toString();
                    const auto lookup_it = lookup_map.find(lookup_key);
                    if (lookup_it == lookup_map.cend())
                        continue;

                    // 直接使用缓存的asset_type关联资产
                    AssetType asset_type = pegasus_assets::str_to_type(cached_file["asset_type"].toString());
                    lookup_it->second->assetsMut().add_file(asset_type, file_path);
                }
                continue;
            }

            // 步骤3.2：扫描未缓存或变更的文件（原逻辑优化）
            QDirIterator dir_it(media_dir, dir_filters, dir_flags);
            QJsonArray new_cached_files;
            while (dir_it.hasNext()) {
                dir_it.next();
                const QFileInfo fileinfo = dir_it.fileInfo();
                const QString file_path = dir_it.filePath();

                // 原逻辑：检测文件类型并关联游戏
                const QString lookup_key = ::clean_abs_dir(fileinfo).remove(dir_base.length(), media_subdir_name.size());
                const auto lookup_it = lookup_map.find(lookup_key);
                if (lookup_it == lookup_map.cend())
                    continue;

                const AssetType asset_type = detect_asset_type(fileinfo.completeBaseName(), fileinfo.suffix());
                if (asset_type == AssetType::UNKNOWN)
                    continue;

                lookup_it->second->assetsMut().add_file(asset_type, file_path);

                QJsonObject new_cached_file{
                    {"file_path", file_path},
                    {"asset_type", pegasus_assets::type_to_str(asset_type)},
                    {"mtime", fileinfo.lastModified().toSecsSinceEpoch()},
                    {"size", fileinfo.size()},
                    {"game_key", lookup_key}
                };
                new_cached_files.append(new_cached_file);
            }

            // 步骤3.3：更新缓存目录记录（优化逻辑）
            QJsonObject new_dir{
                {"dir_path", media_dir},
                {"last_scan_time", QDateTime::currentDateTime().toString(Qt::ISODate)},
                {"files", new_cached_files}
            };
            cached_media_dirs.append(new_dir);
        }
    }

    // 步骤4：保存更新后的缓存
    m_cache_data["media_dirs"] = cached_media_dirs;
    save_cache();

    return *this;
}

void MediaProvider::load_cache()
{
    QFile file(m_cache_path);
    if (!file.open(QIODevice::ReadOnly)) {
        Log::info(LOGMSG("No existing media cache found, will create a new one."));
        m_cache_data = QJsonObject{{"version", "1.0"}, {"media_dirs", QJsonArray{}}};
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        Log::warning(LOGMSG("Invalid media cache format, will reset."));
        m_cache_data = QJsonObject{{"version", "1.0"}, {"media_dirs", QJsonArray{}}};
        return;
    }

    m_cache_data = doc.object();
    // 检查缓存版本，若不匹配则重置（示例：仅支持1.0）
    if (m_cache_data["version"].toString() != "1.0") {
        Log::info(LOGMSG("Cache version mismatch, resetting."));
        m_cache_data = QJsonObject{{"version", "1.0"}, {"media_dirs", QJsonArray{}}};
    }
}

void MediaProvider::save_cache()
{
    QJsonDocument doc(m_cache_data);
    QFile file(m_cache_path);
    if (!file.open(QIODevice::WriteOnly)) {
        Log::warning(LOGMSG("Failed to save media cache to %1").arg(m_cache_path));
        return;
    }
    file.write(doc.toJson());
}

} // namespace media
} // namespace providers
