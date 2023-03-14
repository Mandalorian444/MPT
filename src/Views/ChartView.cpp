#include "ChartView.h"

#include <implot.h>


void ChartView::onImGuiRender(Application& app)
{
    Model& model = app.getModel();
    if (ImGui::Begin(getID().c_str(), &_open))
    {
        try
        {
            const Item& item = model.getItem(model.getSelectedItem());
            const std::vector<Entry>& entries = item.getEntries();
            if (ImPlot::BeginPlot("Price/Time", ImGui::GetContentRegionAvail()))
            {
                ImPlot::SetupAxes("Date", "Price $(USD)");
                ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
                ImPlot::SetupFinish();

                const int entryCount = entries.size();
                std::vector<float> entryPrices;
                std::vector<float> entryDates;
                size_t selectedIndex = 0ull;
                size_t index = 0ull;
                for (auto& entry : entries)
                {
                    entryPrices.push_back(entry.getPrice());
                    std::tm entryDate = entry.getDate().getDate();
                    entryDates.push_back(static_cast<float>(std::mktime(&entryDate)));
                    if (entry == model.getSelectedEntry())
                    {
                        selectedIndex = index;
                    }
                    ++index;
                }

                if (entryCount == 1)
                {
                    ImPlot::PlotScatter(
                        model.getSelectedItem().c_str(),
                        &entryDates[0],
                        &entryPrices[0],
                        entryCount
                    );
                }
                else
                {
                    ImPlot::PlotLine(
                        model.getSelectedItem().c_str(),
                        &entryDates[0],
                        &entryPrices[0],
                        entryCount
                    );
                    ImPlot::PlotScatter(
                        "##SelectedEntry",
                        &entryDates[selectedIndex],
                        &entryPrices[selectedIndex],
                        1
                    );
                }
                ImPlot::EndPlot();
            }
        }
        catch (const std::exception&)
        {
            //  Don't chart anything
        }
    }
    ImGui::End();
}
