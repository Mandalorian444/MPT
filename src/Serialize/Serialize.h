#pragma once

#include <filesystem>
#include <functional>
#include <map>

#include <DataClasses/Item.h>


namespace MPT
{
    void Deserialize(
        const std::filesystem::path& filepath,
        std::map<std::string, Item>& items,
        const std::function<void(const std::string&, Entry)>& addItemCallback
    ) noexcept(false);
    void Serialize(
        const std::filesystem::path& filepath,
        const std::map<std::string, Item>& items
    ) noexcept(false);
}
