#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

#include <DataClasses/Date.h>


namespace MPT
{
    enum class FileDialogPurpose
    {
        Open = 0,
        Save,
        Write
    };

    enum class FileExtension
    {
        MPT = 0
    };

    std::string StringToLower(const std::string& string);
    std::tm DateToTm(const Date& date);
    Date TmToDate(const std::tm& tm);

    std::vector<std::filesystem::path> GetRootDirs();
    std::vector<std::filesystem::path> GetDirContents(const std::filesystem::path& directory);
    size_t GetFileSize(const std::filesystem::path& file);
    std::tm GetFileCreatedDate(const std::filesystem::path& file);
    std::tm GetFileLastWriteDate(const std::filesystem::path& file);
    //  Return size as human readable string
    std::string GetFileSizeString(const std::filesystem::path& file);

    class FileType
    {
    private:
        const std::string _name;
        const std::vector<std::string> _extensions;

    public:
        FileType(
            const std::string& name,
            const std::vector<std::string>& extensions
        )
          : _name(name),
            _extensions(extensions)
        {}
        explicit FileType(const MPT::FileType& type)
          : _name(type._name),
            _extensions(type._extensions)
        {}

        inline std::string getName() const noexcept { return _name; }
        inline const std::vector<std::string>& getExtensions() const noexcept
        {
            return _extensions;
        }

        inline bool operator==(const FileType& type) const noexcept
        {
            return _name == type._name &&
                _extensions == type._extensions;
        }
        inline bool operator!=(const FileType& type) const noexcept
        {
            return !(*this == type);
        }
        inline bool operator==(const std::filesystem::path& filepath) const noexcept
        {
            std::string extToLower(MPT::StringToLower(filepath.extension().string()));
            return std::find(
                _extensions.begin(),
                _extensions.end(),
                extToLower
            ) != _extensions.end();
        }
        inline bool operator!=(const std::filesystem::path& filepath) const noexcept
        {
            return !(*this == filepath);
        }
        inline bool operator==(const std::string& extension) const noexcept
        {
            std::string extToLower(MPT::StringToLower(extension));
            return std::find(
                _extensions.begin(),
                _extensions.end(),
                extToLower
            ) != _extensions.end() || extension == _name;
        }
        inline bool operator!=(const std::string& extension) const noexcept
        {
            return !(*this == extension);
        }
        operator std::string() const noexcept
        {
            return _name;
        }
    };

    class FileData
    {
    private:
        size_t _fileSize;
        std::filesystem::path _filepath;
        std::string _fileSizeString;
        std::tm _fileCreatedDate;
        std::tm _fileLastWriteDate;
    public:
        FileData()
          : _filepath(),
            _fileSize(0),
            _fileSizeString(""),
            _fileCreatedDate(),
            _fileLastWriteDate()
        {}

        FileData(
            const std::filesystem::path& path,
            size_t size,
            const std::string& sizeString,
            std::tm createdDate,
            std::tm lastWriteDate
        )
          : _filepath(path),
            _fileSize(size),
            _fileSizeString(sizeString),
            _fileCreatedDate(createdDate),
            _fileLastWriteDate(lastWriteDate)
        {}

        FileData(const std::filesystem::path& path);

        inline std::filesystem::path getFilepath() const noexcept
        {
            return _filepath;
        }
        inline size_t getFileSize() const noexcept { return _fileSize; }
        inline std::string getFileSizeString() const noexcept
        {
            return _fileSizeString;
        }
        inline std::tm getFileCreatedDate() const noexcept
        {
            return _fileCreatedDate;
        }
        std::string getFileCreatedDateString() const noexcept;
        inline std::tm getFileLastWriteDate() const noexcept
        {
            return _fileLastWriteDate;
        }
        std::string getFileLastWriteDateString() const noexcept;

        void setFilepath(const std::filesystem::path& filepath) noexcept;
        inline void setFileSize(const size_t& fileSize) noexcept { _fileSize = fileSize; }
        inline void setFileSizeString(const std::string& sizeString) noexcept
        {
            _fileSizeString = sizeString;
        }
        inline void setFileLastWriteDate(const std::tm& createdDate) noexcept
        {
            _fileLastWriteDate = createdDate;
        }

        inline bool empty() const noexcept { return _filepath.empty(); }
        void clear() noexcept;
        std::string toString() const;

        FileData& operator=(const FileData& file);

        bool operator==(const FileData& file) const noexcept;
        bool operator!=(const FileData& file) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const MPT::FileData& file)
        {
            return os << file.getFilepath().string();
        }
    };

    const std::vector<FileType>* ValidMPTExtensions();
}
