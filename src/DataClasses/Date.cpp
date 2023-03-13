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

std::tm Date::getDate() const noexcept
{
    constexpr const int epoch = 1900;
    std::tm date;
    date.tm_year    = _year.get() - epoch;
    date.tm_mon     = static_cast<int>(_month.get());
    date.tm_mday    = static_cast<int>(_day.get());
    date.tm_sec     = date.tm_min = date.tm_hour = 0;
    return date;
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

bool Date::operator<(const Date& date) const noexcept
{
    if (_year < date._year)
    {
        return true;
    }
    else if (_year == date._year)
    {
        if (_month < date._month)
        {
            return true;
        }
        else if (_month == date._month)
        {
            if (_day < date._day)
            {
                return true;
            }
        }
    }
    return false;
}

bool Date::operator>(const Date& date) const noexcept
{
    return !(*this < date) && !(*this == date);
}

bool Date::operator<=(const Date& date) const noexcept
{
    return !(*this > date);
}

bool Date::operator>=(const Date& date) const noexcept
{
    return !(*this < date);
}

std::tm DateToTm(const Date& date)
{
    std::tm tm;
    tm.tm_mday  = date.getDay().get();
    tm.tm_mon   = date.getMonth().getMonthNumber();
    tm.tm_year  = date.getYear().get();
    return std::tm();
}

Date TmToDate(const std::tm& tm)
{
    if (
        tm.tm_mday < 1  || tm.tm_mday > 31  ||
        tm.tm_mon < 1   || tm.tm_mon > 12   ||
        tm.tm_year < 0
        )
    {
        return Date();
    }
    Date date(
        static_cast<unsigned char>(tm.tm_mday),
        static_cast<Months>(tm.tm_mon - 1),
        tm.tm_year
    );
    return date;
}
