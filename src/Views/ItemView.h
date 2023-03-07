#pragma once

#include <string>

#include <DataClasses/Date.h>
#include <Views/Viewable.h>


enum class PopupOptions : int
{
    All = 0,
    ItemOnly,
    EntryOnly
};

class ItemView : public Viewable
{
private:
    std::string _tempOrigItemName, _tempNewItemName;
    Entry _tempEntry;
    int _selectedEntryIndex;

    void _popupItemFields(
        Application& app,
        PopupOptions options = static_cast<PopupOptions>(0)
    );
public:
    void onImGuiRender(Application& app) override;
};
