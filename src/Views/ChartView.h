#pragma once

#include <Views/Viewable.h>


class ChartView : public Viewable
{
private:
    bool _open = true;
public:
    ChartView(const std::string& id)
      : Viewable(id)
    {}

    void onImGuiRender(Application& app) override;
    virtual bool shouldClose() override { return !_open; }
};
