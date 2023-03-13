#include "Item.h"

const std::vector<Entry> Item::getEntries() const noexcept
{
    return _entries;
}

size_t Item::getEntriesCount() const noexcept
{
    return _entries.size();
}

void Item::addEntry(Entry entry)
{
    _entries.push_back(std::move(entry));
    qsort(
        &_entries[0],
        _entries.size(),
        sizeof(Entry),
        [](const void* a, const void* b)
        {
            const Entry* ap = static_cast<const Entry*>(a);
            const Entry* bp = static_cast<const Entry*>(b);
            if (*ap == *bp)
            {
                return 0;
            }
            else if (*ap < *bp)
            {
                return -1;
            }
            return 1;
        }
    );
}

void Item::editEntry(const size_t index, Entry entry) noexcept(false)
{
    _entries.at(index) = entry;
}

void Item::removeEntry(const size_t& index)
{
    if (index < _entries.size())
    {
        _entries.erase(_entries.begin() + index);
    }
    else
    {
        throw std::exception("Index out of range");
    }
}

float Entry::getPrice() const noexcept
{
    return _price;
}

const std::string& Entry::getStore() const noexcept
{
    return _store;
}

const std::string& Entry::getLocation() const noexcept
{
    return _location;
}

const std::string& Entry::getBrand() const noexcept
{
    return _brand;
}

Date Entry::getDate() const noexcept
{
    return _date;
}

void Entry::setPrice(const float price) noexcept
{
    _price = price;
}

void Entry::setStore(std::string store) noexcept
{
    _store = std::move(store);
}

void Entry::setLocation(std::string location) noexcept
{
    _location = std::move(location);
}

void Entry::setBrand(std::string brand) noexcept
{
    _brand = std::move(brand);
}

void Entry::setDate(Date date) noexcept
{
    _date = std::move(date);
}

void Entry::clear() noexcept
{
    _price = 0.0f;
    _store.clear();
    _location.clear();
    _brand.clear();
    _date.setDate(Date(1, Months::January, 2023));
}

void Entry::operator=(const Entry& entry) noexcept
{
    _price      = entry._price;
    _store      = entry._store;
    _location   = entry._location;
    _brand      = entry._brand;
    _date       = entry._date;
}

bool Entry::operator==(const Entry& entry) const noexcept
{
    return
        _price      == entry._price &&
        _store      == entry._store &&
        _location   == entry._location &&
        _brand      == entry._brand &&
        _date       == entry._date;
}

bool Entry::operator!=(const Entry& entry) const noexcept
{
    return !(*this == entry);
}

bool Entry::operator<(const Entry& entry) const noexcept
{
    if (_brand < entry._brand)
    {
        return true;
    }
    else
    {
        return _date < entry._date;
    }
}

bool Entry::operator>(const Entry& entry) const noexcept
{
    if (_brand > entry._brand)
    {
        return true;
    }
    else
    {
        return _date > entry._date;
    }
}
