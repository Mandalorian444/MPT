#pragma once

#include <Views/Viewable.h>


class ChartView : public Viewable
{
private:
    bool _open = true;
public:
    void onImGuiRender(Application& app) override;
    virtual bool shouldClose() override { return !_open; }
};
