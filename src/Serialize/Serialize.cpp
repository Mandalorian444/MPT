#include "Serialize.h"

#include <fstream>
#include <sstream>

#include <pugixml.hpp>


void MPT::Deserialize(
    const std::filesystem::path& filepath,
    std::unordered_map<std::string, Item>& items,
    const std::function<void(const std::string&, Entry)>& addItemCallback
) noexcept(false)
{
    if (!std::filesystem::exists(filepath))
    {
        std::string except(filepath.string() + " does not exist");
        throw std::exception(except.c_str());
    }

    pugi::xml_document document;
    document.load_file(filepath.c_str());
    for (auto itemIt = document.begin(); itemIt != document.end(); ++itemIt)
    {
        const std::string name = itemIt->name();
        for (auto entryIt = itemIt->begin(); entryIt != itemIt->end(); ++entryIt)
        {
            const float attribPrice =
                entryIt->attribute("Price").as_float();
            const std::string attribStore =
                entryIt->attribute("Store").as_string();
            const std::string attribLocation =
                entryIt->attribute("Location").as_string();
            const std::string attribBrand =
                entryIt->attribute("Brand").as_string();
            const unsigned char attribDay =
                entryIt->attribute("Day").as_uint();
            const unsigned char attribMonth =
                entryIt->attribute("Month").as_uint();
            const int attribYear =
                entryIt->attribute("Year").as_int();

            const Date date(
                attribDay,
                static_cast<Months>(attribMonth),
                attribYear
            );
            const Entry entry(
                attribPrice,
                attribStore,
                attribLocation,
                attribBrand,
                date
            );
            addItemCallback(name, entry);

        }
    }
}

void MPT::Serialize(
    const std::filesystem::path& filepath,
    const std::unordered_map<std::string, Item>& items
) noexcept(false)
{
    std::ofstream outFile;
    outFile.open(filepath);
    if (!outFile)
    {
        std::string except("Could not write to " + filepath.string());
        throw std::exception(except.c_str());
    }

    pugi::xml_document document;
    std::for_each(
        items.begin(),
        items.end(),
        [&](std::pair<std::string, Item> item)
        {
            pugi::xml_node node = document.append_child(item.first.c_str());
            const Item current = item.second;
            const std::vector<Entry>& entries = current.getEntries();
            size_t entryNum = 0ull;
            for (auto it : entries)
            {
                pugi::xml_node entry = node.append_child("");
                pugi::xml_attribute price =
                    entry.append_attribute("Price");
                price.set_value(it.getPrice());
                pugi::xml_attribute store =
                    entry.append_attribute("Store");
                store.set_value(it.getStore().c_str());
                pugi::xml_attribute location =
                    entry.append_attribute("Location");
                location.set_value(it.getLocation().c_str());
                pugi::xml_attribute brand =
                    entry.append_attribute("Brand");
                brand.set_value(it.getBrand().c_str());
                pugi::xml_attribute day =
                    entry.append_attribute("Day");
                day.set_value(it.getDate().getDay().get());
                pugi::xml_attribute month =
                    entry.append_attribute("Month");
                month.set_value(it.getDate().getMonth().getMonthNumber());
                pugi::xml_attribute year =
                    entry.append_attribute("Year");
                year.set_value(it.getDate().getYear().get());
            }
        }
    );
    document.print(outFile);
}
