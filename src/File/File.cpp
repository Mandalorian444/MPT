#include "File.h"

#include <ctime>
#include <sstream>

//  WINDOWS ONLY
#include <Windows.h>
#include <fileapi.h>


static const std::vector<MPT::FileType> mptExtensions
{
    {"mpt", std::vector<std::string>{".mpt"}}
};

std::string MPT::StringToLower(const std::string& string)
{
    std::string stringToLower = string;
    std::transform(
        stringToLower.begin(),
        stringToLower.end(),
        stringToLower.begin(),
        ::tolower
    );
    return std::move(stringToLower);
}

std::tm STimeToTm(LPSYSTEMTIME sTime)
{
    std::tm tm;
    tm.tm_sec     = sTime->wSecond;
    tm.tm_min     = sTime->wMinute;
    tm.tm_hour    = sTime->wHour;
    tm.tm_mday    = sTime->wDay;
    tm.tm_mon     = sTime->wMonth;
    tm.tm_year    = sTime->wYear;
    return tm;
}

const std::vector<MPT::FileType>* MPT::ValidMPTExtensions()
{
    return &mptExtensions;
}

std::vector<std::filesystem::path> MPT::GetRootDirs()
{
    std::vector<std::filesystem::path> rootDirs;
    std::string rootName;
    std::error_code ec;
    for (char ch = 'A'; ch <= 'Z'; ++ch)
    {
        rootName.clear();
        rootName.push_back(ch);
        rootName += ":";
        //  This is needed to got to the actual directory, not some random
        //  place on the drive
        rootName += std::filesystem::path::preferred_separator;
        //  Using ec here captures errors thrown by is_directory, which simply
        //  skips directories that throw errors, such as disk drives
        if (std::filesystem::is_directory(rootName, ec))
        {
            rootDirs.push_back(std::filesystem::path(rootName));
        }
    }
    return rootDirs;
}

std::vector<std::filesystem::path> MPT::GetDirContents(
    const std::filesystem::path& directory
)
{
    std::vector<std::filesystem::path> contents;
    //  Prevent errors incase file is being used by another process
    std::error_code ec;
    for (
        const auto& it : std::filesystem::directory_iterator(
            directory, std::filesystem::directory_options::skip_permission_denied,
            ec
        )
        )
    {
        contents.push_back(it.path());
    }
    return contents;
}

size_t MPT::GetFileSize(const std::filesystem::path& file)
{
    //  Prevent errors incase file is being used by another process
    std::error_code ec;
    if (std::filesystem::is_regular_file(file, ec))
    {
        return std::filesystem::file_size(file);
    }
    return 0;
}

std::tm MPT::GetFileCreatedDate(const std::filesystem::path& file)
{
    //  WINDOWS ONLY
    _WIN32_FILE_ATTRIBUTE_DATA fileData;
    GetFileAttributesEx(file.c_str(), GetFileExInfoStandard, &fileData);
    _SYSTEMTIME sysTime;
    FileTimeToSystemTime(&fileData.ftCreationTime, &sysTime);
    std::tm time = STimeToTm(&sysTime);
    return time;
}

std::tm MPT::GetFileLastWriteDate(const std::filesystem::path& file)
{
    //  WINDOWS ONLY
    _WIN32_FILE_ATTRIBUTE_DATA fileData;
    GetFileAttributesEx(file.c_str(), GetFileExInfoStandard, &fileData);
    _SYSTEMTIME sysTime;
    FileTimeToSystemTime(&fileData.ftLastWriteTime, &sysTime);
    std::tm time = STimeToTm(&sysTime);
    return time;

    //  Some inprecision, but may work cross-platform

    //namespace ch = std::chrono;
    ////  Prevent errors incase file is being used by another process
    //std::error_code ec;
    //std::filesystem::file_time_type lastWriteTime =
    //    std::filesystem::last_write_time(file, ec);
    //const ch::system_clock::time_point clock =
    //    ch::clock_cast<ch::system_clock>(lastWriteTime);
    //const time_t timet = ch::system_clock::to_time_t(clock);
    //std::tm translatedTime;
    //localtime_s(&translatedTime, &timet);
    //translatedTime.tm_year += 1900;
    //return translatedTime;
}

//  Return size as human readable string
std::string MPT::GetFileSizeString(const std::filesystem::path& file)
{
    //  Prevent errors incase file is being used by another process
    std::error_code ec;
    if (std::filesystem::is_regular_file(file, ec))
    {
        //  This part mostly taken from here:
        //  https://en.cppreference.com/w/cpp/filesystem/file_size
        size_t origSize = std::filesystem::file_size(file);
        int i = 0;
        double mantissa = static_cast<double>(origSize);
        for (
            ;
            mantissa >= 1024.0;
            mantissa /= 1024.0, ++i
            );
        mantissa = std::ceil(mantissa * 10.0) / 10;

        //  Remove unnecessary trailing zeros from result
        std::string mantSub = std::to_string(mantissa);
        size_t zeroes = mantSub.size() - 1;
        for (
            ;
            zeroes != 0 && mantSub[zeroes] == '0';
            --zeroes
            );
        if (mantSub[zeroes] != '.')
        {
            ++zeroes;
        }
        return std::string(std::to_string(mantissa).substr(0, zeroes) + "BKMGTPE"[i]);
    }
    return "";
}

MPT::FileData::FileData(const std::filesystem::path& path)
  : _filepath(path)
{
    _fileSize = GetFileSize(path);
    _fileSizeString = GetFileSizeString(path);
    _fileCreatedDate = GetFileCreatedDate(path);
    _fileLastWriteDate = GetFileLastWriteDate(path);
    std::error_code ec;
    _isDirectory = std::filesystem::is_directory(path, ec);
}

std::string MPT::FileData::getFileCreatedDateString() const noexcept
{
    Date date = TmToDate(_fileCreatedDate);
    return date.getAbbreviatedDateWordString();
}

std::string MPT::FileData::getFileLastWriteDateString() const noexcept
{
    Date date = TmToDate(_fileLastWriteDate);
    return date.getAbbreviatedDateWordString();
}

bool MPT::FileData::getIsDirectory() const noexcept
{
    return _isDirectory;
}

void MPT::FileData::setFilepath(const std::filesystem::path& filepath) noexcept
{
    _filepath = filepath;
}

void MPT::FileData::clear() noexcept
{
    _fileSize = 0;
    _filepath.clear();
    _fileSizeString.clear();
}

std::string MPT::FileData::toString() const
{
    return _filepath.string();
}

MPT::FileData& MPT::FileData::operator=(const FileData& file)
{
    this->_fileSize = file._fileSize;
    this->_filepath = file._filepath;
    this->_fileSizeString = file._fileSizeString;
    this->_fileCreatedDate = file._fileCreatedDate;
    this->_fileLastWriteDate = file._fileLastWriteDate;
    return *this;
}

bool MPT::FileData::operator==(const FileData& file) const noexcept
{
    return _fileSize    == file._fileSize       &&
        _filepath       == file._filepath       &&
        _fileSizeString == file._fileSizeString;
}

bool MPT::FileData::operator!=(const FileData& file) const noexcept
{
    return !(*this == file);
}
