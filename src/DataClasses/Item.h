#pragma once

#include <string>
#include <vector>

#include <DataClasses/Date.h>


class Entry
{
private:
    float _price = 0.0f;
    float _quantity = 1.0f;
    std::string _store;
    std::string _location;
    std::string _brand;
    std::string _itemNumber;
    Date _date;
    //  TODO: create Weight class
    //Weight _weight
    //  Miscellaneous data the user may want to add.
    std::vector<std::string> _other;

public:
    Entry() {}
    Entry(float price, float quantity, std::string store, std::string location, std::string brand, std::string itemNumber, Date date)
      : _price(price),
        _quantity(quantity),
        _store(store),
        _location(location),
        _brand(brand),
        _itemNumber(itemNumber),
        _date(date)
    {}

    float getPrice() const noexcept;
    float getQuantity() const noexcept;
    const std::string& getStore() const noexcept;
    const std::string& getLocation() const noexcept;
    const std::string& getBrand() const noexcept;
    const std::string& getItemNumber() const noexcept;
    Date getDate() const noexcept;
    //Weight getWeight() const noexcept;

    void setPrice(const float price) noexcept;
    void setQuantity(const float quantity) noexcept;
    void setStore(std::string store) noexcept;
    void setLocation(std::string location) noexcept;
    void setBrand(std::string brand) noexcept;
    void setItemNumber(std::string itemNumber) noexcept;
    void setDate(Date date) noexcept;
    //void setWeight(Weight weight) noexcept;

    void clear() noexcept;

    void operator=(const Entry& entry) noexcept;

    bool operator==(const Entry& entry) const noexcept;
    bool operator!=(const Entry& entry) const noexcept;

    bool operator<(const Entry& entry) const noexcept;
    bool operator>(const Entry& entry) const noexcept;
};

class Item
{
private:
    std::vector<Entry> _entries;

public:
    Item(Entry entry)
    {
        _entries.push_back(std::move(entry));
    }

    const std::vector<Entry> getEntries() const noexcept;
    size_t getEntriesCount() const noexcept;

    void addEntry(Entry entry);
    void editEntry(const size_t index, Entry entry) noexcept(false);
    void removeEntry(const size_t& index);
};
