#include "Model.h"

#include <filesystem>

#include <Serialize/Serialize.h>


Item& Model::_getItem(const std::string& item) noexcept(false)
{
    std::unordered_map<std::string, Item>::iterator found = _items.find(item);
    if (found != _items.end())
    {
        return found->second;
    }
    throw std::exception("Item not found");
}

const Item& Model::getItem(const std::string& item) const noexcept(false)
{
    std::unordered_map<std::string, Item>::const_iterator found =
        _items.find(item);
    if (found != _items.end())
    {
        return found->second;
    }
    throw std::exception("Item not found");
}

const std::unordered_map<std::string, Item>& Model::getItems() const noexcept(false)
{
    return _items;
}

size_t Model::getItemsCount() const noexcept
{
    return _items.size();
}

void Model::addItem(const std::string& item, Entry entry) noexcept
{
    try
    {
        auto& found = _items.at(item);
        found.addEntry(entry);
    }
    catch (const std::exception&)
    {
        _items.insert({ item, Item(entry) });
    }
}

void Model::addEntry(const std::string& item, Entry entry) noexcept(false)
{
    _getItem(item).addEntry(entry);
}

void Model::removeEntry(const std::string& item, const size_t& index) noexcept(false)
{
    _getItem(item).removeEntry(index);
}

void Model::save() const
{
    std::filesystem::path filepath("./MPT_save_file.xml");
    MPT::Serialize(filepath, _items);
}

void Model::openProject()
{
    _items.clear();
    std::filesystem::path filepath("./MPT_save_file.xml");
    MPT::Deserialize(
        filepath,
        _items,
        std::bind(
            &Model::addItem,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}
