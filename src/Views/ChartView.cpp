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
                const int entryCount = static_cast<int>(entries.size());
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

                ImPlot::SetupAxes("Date", "Price $(USD)");
                ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
                if (_fit)
                {
                    if (entryCount == 1)
                    {
                        ImPlot::SetupAxesLimits(
                            entryDates[0] - 1.0,
                            entryDates[0] + 1.0,
                            entryPrices[0] - 1.0,
                            entryPrices[0] + 1.0,
                            ImPlotCond_Always
                        );
                    }
                    else
                    {
                        std::pair<float, float> dateMinMax(entryDates[0], entryDates[0]);
                        for (const auto& it : entryDates)
                        {
                            if (it < dateMinMax.first)
                            {
                                dateMinMax.first = it;
                            }
                            if (it > dateMinMax.second)
                            {
                                dateMinMax.second = it;
                            }
                        }
                        std::pair<float, float> priceMinMax(entryPrices[0], entryPrices[0]);
                        for (const auto& it : entryPrices)
                        {
                            if (it < priceMinMax.first)
                            {
                                priceMinMax.first = it;
                            }
                            if (it > priceMinMax.second)
                            {
                                priceMinMax.second = it;
                            }
                        }
                        const double dateMargin =
                            static_cast<double>(
                                entryDates[entries.size() - 1ull] -
                                entryDates[0]
                                ) * 0.1;
                        double priceMargin =
                            static_cast<double>(
                                std::abs(
                                    entryPrices[entries.size() - 1ull] -
                                    entryPrices[0]
                                )
                                ) * 0.1;
                        priceMargin = priceMargin > 0.0 ? priceMargin : 1.0;
                        ImPlot::SetupAxesLimits(
                            dateMinMax.first - dateMargin,
                            dateMinMax.second + dateMargin,
                            priceMinMax.first - priceMargin,
                            priceMinMax.second + priceMargin,
                            ImPlotCond_Always
                        );
                    }
                    _fit = false;
                }
                ImPlot::SetupFinish();

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
                if (ImPlot::IsPlotHovered() && ImGui::IsKeyPressed(ImGuiKey_F))
                {
                    _fit = true;
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
