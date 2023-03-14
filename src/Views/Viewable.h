#pragma once

#include <string>

#include <imgui.h>

#include <Application.h>


enum class ViewNameIndex
{
    ItemView = 0,
    ChartView,
    FileDialogView
};

constexpr const char* ViewNames[]
{
    "Items",
    "Chart",
    "Files"
};

class Viewable
{
private:
    std::string _id;
public:
    Viewable(const std::string& id)
      : _id(id)
    {}

    inline std::string getID() const noexcept { return _id; }

    virtual void onImGuiRender(Application& app) = 0;
    virtual bool shouldClose() = 0;
};
