#include "Model.h"

#include <filesystem>

#include <Serialize/Serialize.h>


Item& Model::_getItem(const std::string& item) noexcept(false)
{
    std::map<std::string, Item>::iterator found = _items.find(item);
    if (found != _items.end())
    {
        return found->second;
    }
    throw std::exception("Item not found");
}

const Item& Model::getItem(const std::string& item) const noexcept(false)
{
    std::map<std::string, Item>::const_iterator found =
        _items.find(item);
    if (found != _items.end())
    {
        return found->second;
    }
    throw std::exception("Item not found");
}

const std::map<std::string, Item>& Model::getItems() const noexcept(false)
{
    return _items;
}

size_t Model::getItemsCount() const noexcept
{
    return _items.size();
}

const std::string& Model::getSelectedItem() const noexcept
{
    return _selectedItem;
}

const Entry& Model::getSelectedEntry() const noexcept
{
    return _selectedEntry;
}

const std::filesystem::path& Model::getProjFilepath() const noexcept
{
    return _projFilepath;
}

void Model::setSelectedItem(const std::string& item) noexcept
{
    _selectedItem = item;
}

void Model::setSelectedEntry(const Entry& entry) noexcept
{
    _selectedEntry = entry;
}

void Model::deselectedCurrentEntry() noexcept
{
    _selectedEntry.clear();
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

void Model::editItem(
    const std::string& origItem,
    const std::string& newItem
) noexcept
{
    const auto itemRange = _items.equal_range(origItem);
    std::pair<std::string, Item> item = *itemRange.first;
    item.first = newItem;
    _items.erase(itemRange.first);
    _items.insert(item);
}

void Model::editEntry(
    const std::string & item,
    const size_t entryIndex,
    Entry entry
) noexcept(false)
{
    _getItem(item).editEntry(entryIndex, entry);
}

void Model::removeEntry(const std::string& item, const size_t& index) noexcept(false)
{
    _getItem(item).removeEntry(index);
}

void Model::save() const noexcept(false)
{
    if (!_projFilepath.empty())
    {
        MPT::Serialize(_projFilepath, _items);
    }
    else
    {
        throw std::exception("Project not yet saved");
    }
}

void Model::saveAs(const std::filesystem::path& filepath)
{
    if (!filepath.empty())
    {
        _projFilepath = filepath;
        MPT::Serialize(_projFilepath, _items);
    }
    else
    {
        throw std::exception("No filepath provided");
    }
}

void Model::openProject(std::filesystem::path filepath)
{
    _items.clear();
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
    _projFilepath = filepath;
}
