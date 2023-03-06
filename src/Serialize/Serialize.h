#pragma once

#include <filesystem>
#include <functional>
#include <unordered_map>

#include <DataClasses/Item.h>


namespace MPT
{
    void Deserialize(
        const std::filesystem::path& filepath,
        std::unordered_map<std::string, Item>& items,
        const std::function<void(const std::string&, Entry)>& addItemCallback
    ) noexcept(false);
    void Serialize(
        const std::filesystem::path& filepath,
        const std::unordered_map<std::string, Item>& items
    ) noexcept(false);
}
