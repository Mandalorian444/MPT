#include "Date.h"


std::string Date::_getDateString(const bool abbreviated) const noexcept
{
    std::string month;
    if (abbreviated)
    {
        month = AbbreviatedMonthNames[_month];
    }
    else
    {
        month = MonthNames[_month];
    }
    return month + " " +
        std::to_string(_day.get()) + ", " +
        std::to_string(_year.get());
}

void Date::setDate(const Date& date) noexcept
{
    _day = date._day;
    _month = date._month;
    _year = date._year;
}

std::string Date::getDateNumberString() const noexcept
{
    return
        std::to_string(_day.get()) + "/" +
        std::to_string(_month.getMonthNumber()) + "/" +
        std::to_string(_year.get())
        ;
}

std::string Date::getDateWordString() const noexcept
{
    return _getDateString(false);
}

std::string Date::getAbbreviatedDateWordString() const noexcept
{
    return _getDateString(true);
}

bool Date::operator==(const Date& date) const noexcept
{
    return
        _day    == date._day &&
        _month  == date._month &&
        _year   == date._year;
}

bool Date::operator!=(const Date& date) const noexcept
{
    return !(*this == date);
}
