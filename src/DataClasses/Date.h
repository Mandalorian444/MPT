#pragma once

#include <string>
#include <utility>


enum class Months
{
    January = 0,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December,
    Count
};

constexpr const char* MonthNames[] =
{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

constexpr const char* AbbreviatedMonthNames[] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sept",
    "Oct",
    "Nov",
    "Dec"
};

constexpr std::pair<unsigned char, unsigned char> MonthDayRange[] =
{
    {1, 31},    //  Jan
    {1, 28},    //  Feb
    {1, 31},    //  Mar
    {1, 30},    //  Apr
    {1, 31},    //  May
    {1, 30},    //  Jun
    {1, 31},    //  Jul
    {1, 31},    //  Aug
    {1, 30},    //  Sept
    {1, 31},    //  Oct
    {1, 30},    //  Nov
    {1, 30}     //  Dec
};

class Day
{
private:
    unsigned char _day;

public:
    Day()
        : _day(1)
    {}
    Day(unsigned char day)
        : _day(day)
    {}

    inline void set(unsigned char day) noexcept { _day = day; }

    inline unsigned char get() const noexcept { return _day; }

    inline operator int() const noexcept { return static_cast<int>(_day); }

    inline void operator=(const Day& day) noexcept { _day = day._day; }

    inline bool operator==(const Day& day) const noexcept { return _day == day._day; }
    inline bool operator!=(const Day& day) const noexcept { return !(*this == day); }
    inline bool operator<(const Day& day) const noexcept { return _day < day._day; }
    inline bool operator>(const Day& day) const noexcept { return _day > day._day; }
    inline bool operator<=(const Day& day) const noexcept { return *this < day || *this == day; }
    inline bool operator>=(const Day& day) const noexcept { return *this > day || *this == day; }
};

class Month
{
private:
    Months _month;

public:
    Month()
        : _month(Months::January)
    {}
    Month(Months month)
        : _month(month)
    {}
    Month(const Month& month)
        : _month(month._month)
    {}
    inline void set(const Months& month) noexcept { _month = month; }

    inline Months get() const noexcept { return _month; }
    inline unsigned char getMonthNumber() const noexcept { return static_cast<unsigned char>(_month) + 1; }
    inline const char* getName() const noexcept { return MonthNames[static_cast<int>(_month)]; }
    inline const char* getAbbreviatedName() const noexcept { return AbbreviatedMonthNames[static_cast<int>(_month)]; }

    inline operator int() const noexcept { return static_cast<int>(_month); }

    inline void operator=(const Month& month) noexcept { _month = month._month; }

    inline bool operator==(const Month& month) const noexcept { return _month == month._month; }
    inline bool operator!=(const Month& month) const noexcept { return !(*this == month); }
    inline bool operator<(const Month& month) const noexcept { return _month < month._month; }
    inline bool operator>(const Month& month) const noexcept { return _month > month._month; }
    inline bool operator<=(const Month& month) const noexcept { return *this < month || *this == month; }
    inline bool operator>=(const Month& month) const noexcept { return *this > month || *this == month; }
};

class Year
{
private:
    int _year;

public:
    Year()
        : _year(0)
    {}
    Year(int year)
        : _year(year)
    {}
    Year(const Year& year)
        : _year(year._year)
    {}

    inline void set(int year) noexcept { _year = year; }

    inline int get() const noexcept { return _year; }

    inline operator int() const noexcept { return _year; }

    inline void operator=(int year) noexcept { _year = year; }

    inline bool operator==(int year) const noexcept { return _year == year; }
    inline bool operator!=(int year) const noexcept { return !(*this == year); }
    inline bool operator<(int year) const noexcept { return _year < year; }
    inline bool operator>(int year) const noexcept { return _year > year; }
    inline bool operator<=(int year) const noexcept { return *this < year || *this == year; }
    inline bool operator>=(int year) const noexcept { return *this > year || *this == year; }

    inline bool operator==(const Year& year) const noexcept { return *this == year._year; }
    inline bool operator!=(const Year& year) const noexcept { return !(*this == year); }
    inline bool operator<(const Year& year) const noexcept { return *this < year; }
    inline bool operator>(const Year& year) const noexcept { return *this > year; }
    inline bool operator<=(const Year& year) const noexcept { return *this < year || *this == year; }
    inline bool operator>=(const Year& year) const noexcept { return *this > year || *this == year; }
};

class Date
{
private:
    Day _day;
    Month _month;
    Year _year;

    std::string _getDateString(const bool abbreviated) const noexcept;
public:
    Date()
    {}
    Date(unsigned char day, Months month, int year)
      : _day(day),
        _month(month),
        _year(year)
    {}
    Date(const Date& date)
      : _day(date._day),
        _month(date._month),
        _year(date._year)
    {}

    void setDate(const Date& date) noexcept;

    inline Day getDay() const noexcept { return _day; }
    inline Month getMonth() const noexcept { return _month; }
    inline Year getYear() const noexcept { return _year; }

    //  DD/MM/YYYY
    std::string getDateNumberString() const noexcept;
    //  MM DD, YYYY
    std::string getDateWordString() const noexcept;
    std::string getAbbreviatedDateWordString() const noexcept;
};
