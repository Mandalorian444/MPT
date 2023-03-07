#pragma once

#include <string>

#include <DataClasses/Date.h>
#include <Views/Viewable.h>


class ItemView : public Viewable
{
private:
    std::string _tempItemName;
    Entry _tempEntry;
    std::string _selectedItem;
    Entry _selectedEntry;

    void _popupItemFields(Application& app);
public:
    void onImGuiRender(Application& app) override;
};
