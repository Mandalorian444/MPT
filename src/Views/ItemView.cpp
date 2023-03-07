#include "ItemView.h"

//  For InputText to use std::string
#include <imgui_stdlib.h>

#include <Views/PopUps.h>
#include <DataClasses/Date.h>


enum class Column: int
{
    Item = 0,
    Date,
    Price,
    Store,
    Location,
    Brand,
    Weight,
    Count
};

void ItemView::_popupItemFields(Application& app)
{
    ImGuiInputTextFlags textInputFlags = 0
        //| ImGuiInputTextFlags_EnterReturnsTrue
        ;
    ImGui::Text("Item Name");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    std::string itemName = _tempItemName;
    if (ImGui::InputText("##ItemNameInput", &itemName, textInputFlags))
    {
        _tempItemName = itemName;
    }

    ImGui::Text("Price");
    ImGui::SameLine();
    float price = _tempEntry.getPrice();
    if (ImGui::InputFloat("##PriceInput", &price, 0.01f, 10.0f, "$%.2f", textInputFlags | ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank))
    {
        _tempEntry.setPrice(price);
    }

    ImGui::Text("Day");
    ImGui::SameLine();
    const int day = _tempEntry.getDate().getDay().get();
    ImGui::SetNextItemWidth(ImGui::CalcTextSize("000000").x);
    if (ImGui::BeginCombo("##DayCombo", std::to_string(day).c_str()))
    {
        bool selectedDay = false;
        for (unsigned char i = 1u; i <= MonthDayRange[static_cast<unsigned char>(_tempEntry.getDate().getMonth().get())].second; ++i)
        {
            if (_tempEntry.getDate().getDay().get() == i)
            {
                selectedDay = true;
            }

            std::string dayNum(std::to_string(i));
            if (ImGui::Selectable(dayNum.c_str(), &selectedDay))
            {
                _tempEntry.setDate(
                    {
                        i,
                        _tempEntry.getDate().getMonth().get(),
                        _tempEntry.getDate().getYear().get()
                    }
                );
            }
            selectedDay = false;
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    ImGui::Text("Month");
    ImGui::SetNextItemWidth(ImGui::CalcTextSize("December+0000").x);
    ImGui::SameLine();
    if (ImGui::BeginCombo("##MonthCombo", _tempEntry.getDate().getMonth().getAbbreviatedName()))
    {
        bool selectedMonth = false;
        for (unsigned char i = 0u; i < static_cast<unsigned char>(Months::Count); ++i)
        {
            if (i == static_cast<unsigned char>(_tempEntry.getDate().getMonth().get()))
            {
                selectedMonth = true;
            }

            if (ImGui::Selectable(MonthNames[i], &selectedMonth))
            {
                _tempEntry.setDate(
                    {
                        _tempEntry.getDate().getDay().get(),
                        static_cast<Months>(i),
                        _tempEntry.getDate().getYear().get()
                    }
                );
            }
            selectedMonth = false;
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    ImGui::Text("Year");
    ImGui::SameLine();
    int year = _tempEntry.getDate().getYear().get();
    ImGui::SetNextItemWidth(ImGui::CalcTextSize("9999+0000+0000").x);
    if (ImGui::InputInt("##YearInput", &year))
    {
        _tempEntry.setDate(
            {
                _tempEntry.getDate().getDay().get(),
                _tempEntry.getDate().getMonth().get(),
                year
            }
        );
    }

    ImGui::Text("Brand");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    std::string brand = _tempEntry.getBrand();
    if (ImGui::InputText("##ItemBrandInput", &brand, textInputFlags))
    {
        _tempEntry.setBrand(brand);
    }

    ImGui::Text("Store");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    std::string store = _tempEntry.getStore();
    if (ImGui::InputText("##ItemStoreInput", &store, textInputFlags))
    {
        _tempEntry.setStore(store);
    }

    ImGui::Text("Location");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    std::string location = _tempEntry.getLocation();
    if (ImGui::InputText("##ItemLocationInput", &location, textInputFlags))
    {
        _tempEntry.setLocation(location);
    }
}

void ItemView::onImGuiRender(Application& app)
{
    if (ImGui::Begin("ItemView"))
    {
        ImGuiStyle& style = ImGui::GetStyle();
        const ImVec2 textSize = ImGui::CalcTextSize("Add Item");
        const float width = ImGui::GetContentRegionAvail().x;
        //  Leave space for the "Add Item" button
        const float height = ImGui::GetContentRegionAvail().y - (textSize.y + style.FramePadding.y * 2.0f) - style.ItemSpacing.y;
        ImGuiTableFlags flags =
            ImGuiTableFlags_ScrollX
            | ImGuiTableFlags_ScrollY
            | ImGuiTableFlags_Resizable;
        if (ImGui::BeginTable("##ItemTable", static_cast<int>(Column::Count), flags, ImVec2(width, height)))
        {
            ImGui::TableSetupScrollFreeze(static_cast<int>(Column::Count), 1);
            ImGui::TableSetupColumn("Item");
            ImGui::TableSetupColumn("Date");
            ImGui::TableSetupColumn("Price");
            ImGui::TableSetupColumn("Store");
            ImGui::TableSetupColumn("Location");
            ImGui::TableSetupColumn("Brand");
            ImGui::TableSetupColumn("Weight");
            ImGui::TableHeadersRow();

            auto& items = app.getModel().getItems();
            int itemCount = 0;
            for (auto it : items)
            {
                ImGui::PushID(itemCount);
                bool selectedItem = it.first == _selectedItem;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(static_cast<int>(Column::Item));
                if (ImGui::Selectable(it.first.c_str(), &selectedItem, ImGuiSelectableFlags_SpanAllColumns))
                {
                    _selectedItem = it.first;
                    _selectedEntry.clear();
                }

                const std::vector<Entry>& entries = it.second.getEntries();
                int entryCount = 0;
                for (auto entry : entries)
                {
                    ImGui::PushID(entryCount);
                    bool selectedEntry = entry == _selectedEntry;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(static_cast<int>(Column::Date));
                    if (ImGui::Selectable(entry.getDate().getAbbreviatedDateWordString().c_str(), &selectedEntry, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
                    {
                        _selectedItem = it.first;
                        _selectedEntry = entry;
                    }
                    //ImGui::Text(entry.getDate().getAbbreviatedDateWordString().c_str());

                    ImGui::TableSetColumnIndex(static_cast<int>(Column::Price));
                    std::string price = std::to_string(entry.getPrice());
                    //  Format our price with dollar sign and at least 2 decimal places
                    ImGui::Text(("$" + price.substr(0, price.find('.') + 3u)).c_str());

                    ImGui::TableSetColumnIndex(static_cast<int>(Column::Store));
                    ImGui::Text(entry.getStore().c_str());

                    ImGui::TableSetColumnIndex(static_cast<int>(Column::Location));
                    ImGui::Text(entry.getLocation().c_str());

                    ImGui::TableSetColumnIndex(static_cast<int>(Column::Brand));
                    ImGui::Text(entry.getBrand().c_str());

                    //ImGui::TableSetColumnIndex(static_cast<int>(Column::Weight));
                    //ImGui::Text(entry.getWeight());

                    ImGui::PopID();
                    ++entryCount;
                }
                ImGui::PopID();
                ++itemCount;
            }
            ImGui::EndTable();
        }
        if (ImGui::Button("+ Add item"))
        {
            ImGui::OpenPopup(PopupNames[static_cast<int>(Popup::AddItem)]);
        }
        ImGui::SameLine();
        if (ImGui::Button("Edit item"))
        {
            ImGui::OpenPopup(PopupNames[static_cast<int>(Popup::EditItem)]);
            _tempItemName = _selectedItem;
            _tempEntry = _selectedEntry;
        }

        if (ImGui::IsPopupOpen(PopupNames[static_cast<int>(Popup::AddItem)]))
        {
            if (ImGui::BeginPopupModal(PopupNames[static_cast<int>(Popup::AddItem)]))
            {
                _popupItemFields(app);

                if (ImGui::Button("Add Item"))
                {
                    app.getModel().addItem(_tempItemName, _tempEntry);
                    _tempItemName.clear();
                    _tempEntry.clear();
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("Close popup"))
                {
                    _tempItemName.clear();
                    _tempEntry.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        else if (ImGui::IsPopupOpen(PopupNames[static_cast<int>(Popup::EditItem)]))
        {
            if (ImGui::BeginPopupModal(PopupNames[static_cast<int>(Popup::EditItem)]))
            {
                _popupItemFields(app);

                if (ImGui::Button("Save"))
                {
                    //  TODO: save edits
                    _tempItemName.clear();
                    _tempEntry.clear();
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("Cancel"))
                {
                    _tempItemName.clear();
                    _tempEntry.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        else if (ImGui::IsPopupOpen(PopupNames[static_cast<int>(Popup::EditEntry)]))
        {
            if (ImGui::BeginPopupModal(PopupNames[static_cast<int>(Popup::EditEntry)]))
            {
                _popupItemFields(app);

                if (ImGui::Button("Save"))
                {
                    //  TODO: save edits
                    _tempItemName.clear();
                    _tempEntry.clear();
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("Cancel"))
                {
                    _tempItemName.clear();
                    _tempEntry.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }
    ImGui::End();
}
