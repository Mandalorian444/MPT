#pragma once


enum class Popup: int
{
    AddItem = 0,
    AddEntry,
    EditItem,
    EditEntry,
    RemoveItem,
    RemoveEntry,
    SaveOverwrite,
    Count
};

constexpr const char* PopupNames[] =
{
    "Add Item",
    "Add Entry",
    "Edit Item",
    "Edit Entry",
    "Remove Item",
    "Remove Entry",
    "Overwrite"
};
