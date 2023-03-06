#pragma once

#include <string>
#include <vector>

#include <DataClasses/Date.h>


class Entry
{
private:
    float _price;
    std::string _store;
    std::string _location;
    std::string _brand;
    Date _date;
    //  TODO: create Weight class
    //Weight _weight
    //  Miscellaneous data the user may want to add.
    std::vector<std::string> _other;

public:
    Entry() {}
    Entry(float price, std::string store, std::string location, std::string brand, Date date)
      : _price(price),
        _store(store),
        _location(location),
        _brand(brand),
        _date(date)
    {}

    float getPrice() const noexcept;
    const std::string& getStore() const noexcept;
    const std::string& getLocation() const noexcept;
    const std::string& getBrand() const noexcept;
    Date getDate() const noexcept;
    //Weight getWeight() const noexcept;

    void setPrice(const float price) noexcept;
    void setStore(std::string store) noexcept;
    void setLocation(std::string location) noexcept;
    void setBrand(std::string brand) noexcept;
    void setDate(Date date) noexcept;
    //void setWeight(Weight weight) noexcept;

    void clear() noexcept;
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
    void removeEntry(const size_t& index);
};
