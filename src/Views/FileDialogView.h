#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <File/File.h>
#include <Views/Viewable.h>


//  DO NOT use outside of FileDialogView
class FileDialogHistory
{
private:
    std::vector<std::filesystem::path> _history;
    //  _it points the current location
    std::vector<std::filesystem::path>::iterator _it = _history.begin();
public:
    void addToHistory(const std::filesystem::path& dir);
    std::filesystem::path back(const std::filesystem::path& dir);
    std::filesystem::path forward(const std::filesystem::path& dir);
};

class FileDialogView : public Viewable
{
private:
    //  Take this as an argument so the opening view has control
    bool _open = true;
    //  TODO: save this state so it's consistent when opening again?
    bool _showSequences = true;
    float _separatorX;
    MPT::FileDialogPurpose _purpose;
    std::string _buttonText;
    std::filesystem::path _currentDir;
    std::vector<MPT::FileData> _dirContents;
    //  This is what the input path is set to when chosen
    //  Always clear this if _currentDir changes
    MPT::FileData _selected;
    std::vector<std::filesystem::path> _rootDirs;
    FileDialogHistory _history;
    //  TODO: make this handle multiple extensions (e.g. .tif, .tiff, .TIFF)
    std::string _filterSelection;
    const std::vector<MPT::FileType>* _extensionOptions;
    std::vector<MPT::FileType> _extensionFilter;

public:
    FileDialogView();
    FileDialogView(
        std::filesystem::path inputDir,
        MPT::FileDialogPurpose purpose,
        MPT::FileExtension fileExtension
    );
    FileDialogView(const FileDialogView& dialog);
    ~FileDialogView();
    void onImGuiRender(Application& app) override;

    virtual bool shouldClose() override { return !_open; }

    FileDialogView& operator=(const FileDialogView& dialog);
};

bool Splitter(
    bool split_vertically,
    float thickness,
    float* size1,
    float* size2,
    float min_size1,
    float min_size2,
    float splitter_long_axis_size = -1.0f
);
