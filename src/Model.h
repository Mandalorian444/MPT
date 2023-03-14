#pragma once

#include <filesystem>
#include <map>

#include <DataClasses/Item.h>


class Model
{
private:
    std::map<std::string, Item> _items;
    std::string _selectedItem;
    Entry _selectedEntry;
    std::filesystem::path _projFilepath;

    Item& _getItem(const std::string& item) noexcept(false);
public:
    const Item& getItem(const std::string& item) const noexcept(false);
    const std::map<std::string, Item>& getItems() const noexcept(false);
    size_t getItemsCount() const noexcept;
    const std::string& getSelectedItem() const noexcept;
    const Entry& getSelectedEntry() const noexcept;
    const std::filesystem::path& getProjFilepath() const noexcept;

    void setSelectedItem(const std::string& item) noexcept;
    void setSelectedEntry(const Entry& entry) noexcept;
    void deselectedCurrentEntry() noexcept;

    void addItem(const std::string& item, Entry entry) noexcept;
    void addEntry(const std::string& item, Entry entry) noexcept(false);
    void editItem(
        const std::string& origItem,
        const std::string& newItem
    ) noexcept;
    void editEntry(
        const std::string& item,
        const size_t entryIndex,
        Entry entry
    ) noexcept(false);
    void removeEntry(const std::string& item, const size_t& index) noexcept(false);

    void save() const noexcept(false);
    void saveAs(const std::filesystem::path& filepath);
    //void newProject();
    void openProject(std::filesystem::path filepath);
    //void closeProject();
};
