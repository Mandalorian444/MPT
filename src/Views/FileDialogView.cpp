#include "FileDialogView.h"

#include <algorithm>

#include <imgui.h>
#include <imgui_internal.h>
//  For string version of ImGui::InputText
#include <imgui_stdlib.h>

#include <File/File.h>
#include <Views/PopUps.h>

#define DEFAULT_FILTER_SELECTION ".* (all)"


void FillDirContents(
    const std::filesystem::path& dir,
    std::vector<MPT::FileData>& contents
)
{
    for (
        const auto& it : std::filesystem::directory_iterator(
            dir, std::filesystem::directory_options::skip_permission_denied
        )
    )
    {
        if (it.is_regular_file())
        {
            const std::filesystem::path filepath = it.path();
            contents.push_back(
                MPT::FileData(
                    filepath,
                    MPT::GetFileSize(filepath),
                    MPT::GetFileSizeString(filepath),
                    MPT::GetFileCreatedDate(filepath),
                    MPT::GetFileLastWriteDate(filepath)
                )
            );
        }
        else
        {
            contents.push_back(MPT::FileData(it.path()));
        }
    }
}

void FilterDirsToTop(std::vector<MPT::FileData>& contents)
{
    std::error_code ec;
    std::vector<MPT::FileData> directories;
    std::vector<MPT::FileData> files;
    directories.reserve(contents.size());
    files.reserve(contents.size());

    auto contIt = contents.begin();
    for (; contIt != contents.end(); ++contIt)
    {
        if (std::filesystem::is_directory(contIt->getFilepath(), ec))
        {
            directories.push_back(*contIt);
        }
        else
        {
            files.push_back(*contIt);
        }
    }

    assert(directories.size() + files.size() == contents.size());

    contIt = contents.begin();
    for (
        auto dirIt = directories.begin();
        dirIt != directories.end();
        ++dirIt, ++contIt
        )
    {
        *contIt = *dirIt;
    }
    for (auto filIt = files.begin(); filIt != files.end(); ++filIt, ++contIt)
    {
        *contIt = *filIt;
    }
}

void FilterByExtension(
    std::vector<MPT::FileData>& contents,
    const std::vector<MPT::FileType>& extensions
)
{
    std::error_code ec;
    for (auto itCont = contents.begin(); itCont != contents.end();)
    {
        if (std::filesystem::is_regular_file(itCont->getFilepath(), ec))
        {
            bool valid = false;
            for (
                auto itExt = extensions.begin();
                itExt != extensions.end();
                ++itExt
                )
            {
                if (*itExt == itCont->getFilepath().extension().string())
                {
                    valid = true;
                    ++itCont;
                    break;
                }
            }
            if (!valid)
            {
                itCont = contents.erase(itCont);
            }
        }
        else
        {
            ++itCont;
        }
    }
}

void RefreshCurrentDirectory(
    const std::filesystem::path& dir,
    std::vector<MPT::FileData>& contents,
    const std::vector<MPT::FileType>& extensions
)
{
    contents.clear();
    FillDirContents(dir, contents);
    FilterByExtension(contents, extensions);
    FilterDirsToTop(contents);
}

void FileDialogHistory::addToHistory(const std::filesystem::path& dir)
{
    if (_history.size() == 0)
    {
        _history.push_back(dir);
        _it = _history.begin();
    }
    else if (*_it != dir)
    {
        _history.erase(++_it, _history.end());
        _history.push_back(dir);
        _it = --_history.end();
    }
    //  Don't do anything if dir is the same as _it
}

std::filesystem::path FileDialogHistory::back(const std::filesystem::path& dir)
{
    if (_history.size() == 0)
    {
        addToHistory(dir);
    }
    else if (_it == --_history.end() && *_it != dir)
    {
        addToHistory(dir);
    }
    if (_it != _history.begin())
    {
        --_it;
    }
    return *_it;
}

std::filesystem::path FileDialogHistory::forward(const std::filesystem::path& dir)
{
    if (_it != --_history.end())
    {
        ++_it;
    }
    else if (*_it != dir)
    {
        addToHistory(dir);
    }
    return *_it;
}

FileDialogView::FileDialogView()
  : _open(false),
    _separatorX(50.0f),
    _purpose(MPT::FileDialogPurpose::Open),
    _rootDirs(MPT::GetRootDirs()),
    _filterSelection(DEFAULT_FILTER_SELECTION),
    _extensionOptions(nullptr)
{}

FileDialogView::FileDialogView(
    std::filesystem::path inputDir,
    MPT::FileDialogPurpose purpose,
    MPT::FileExtension fileExtension
)
  : _open(true),
    _separatorX(50.0f),
    _purpose(purpose),
    _currentDir(inputDir.parent_path()),
    _selected(inputDir),
    _rootDirs(MPT::GetRootDirs()),
    _filterSelection(DEFAULT_FILTER_SELECTION),
    _extensionOptions(nullptr)
{
    switch (purpose)
    {
    case MPT::FileDialogPurpose::Open:
        _buttonText = "Open";
        break;
    case MPT::FileDialogPurpose::Save:
        _buttonText = "Save";
        break;
    case MPT::FileDialogPurpose::Write:
        _buttonText = "Set";
        break;
    //  This should never be reached!
    default:
        break;
    }
    switch (fileExtension)
    {
    case MPT::FileExtension::MPT:
        _extensionOptions = MPT::ValidMPTExtensions();
        break;
    //  This should never be reached!
    default:
        break;
    }
    if (_extensionOptions)
    {
        for (auto it = _extensionOptions->begin(); it != _extensionOptions->end(); ++it)
        {
            _extensionFilter.push_back(*it);
        }
    }
}

FileDialogView::FileDialogView(const FileDialogView& dialog)
  : _open(dialog._open),
    _separatorX(dialog._separatorX),
    _purpose(dialog._purpose),
    _buttonText(dialog._buttonText),
    _currentDir(dialog._currentDir),
    _selected(dialog._selected),
    _rootDirs(dialog._rootDirs),
    _filterSelection(dialog._filterSelection),
    _extensionOptions(dialog._extensionOptions)
{
    if (_extensionOptions)
    {
        for (auto it = _extensionOptions->begin(); it != _extensionOptions->end(); ++it)
        {
            _extensionFilter.push_back(*it);
        }
    }
}

FileDialogView::~FileDialogView()
{}

void FileDialogView::onImGuiRender(Application& app)
{
    std::filesystem::path startDir;
    std::vector<std::filesystem::path> rootDirs(MPT::GetRootDirs());
    if (_currentDir.empty() && rootDirs.empty())
    {
        _open = false;
    }
    else
    {
        startDir = !_currentDir.empty() ?
            _currentDir :
            *rootDirs.begin();
    }

    _currentDir = startDir;

    if (_dirContents.empty())
    {
        RefreshCurrentDirectory(_currentDir, _dirContents, _extensionFilter);
    }

    if(_open)
    {
        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoCollapse
            ;
        //  Set min size to something reasonable to avoid glitchy behavior
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(200, 200));
        if (ImGui::Begin("FileDialog", &_open, windowFlags))
        {
            ImGuiStyle& style = ImGui::GetStyle();
            ImVec2 openCancelSize =
                ImGui::CalcTextSize(
                    "Cancel"
                ) + ImVec2(10.0f, style.FramePadding.y * 2.0f
                );

//**********************************
//  - - - NAVIGATION BUTTONS - - -
//**********************************

            float navButtonSizeFloat =
                ImGui::CalcTextSize("<").y + style.FramePadding.y * 2.0f;

            ImVec2 navButtonSize(navButtonSizeFloat, navButtonSizeFloat);

            if (ImGui::Button("<", navButtonSize))
            {
                _currentDir = _history.back(_currentDir);
                _selected.clear();
            }

            ImGui::SameLine();
            if (ImGui::Button(">", navButtonSize))
            {
                _currentDir = _history.forward(_currentDir);
                _selected.clear();
            }

            ImGui::SameLine();
            if (ImGui::Button("^", navButtonSize))
            {
                if (_currentDir.has_parent_path())
                {
                    _history.addToHistory(_currentDir);
                    _currentDir = _currentDir.parent_path();
                }
                _selected.clear();
            }

//*********************************
//  - - - CURRENT DIRECTORY - - -
//*********************************

            ImGui::SameLine();
            ImGui::Text(_currentDir.string().c_str());
            ImVec2 headingSize = ImGui::GetItemRectSize();

//**********************************
//  - - - DIRECTORY CONTENTS - - -
//**********************************

            ImVec2 avail(
                ImGui::GetContentRegionAvail().x,
                //  Compensate for extra buttons and fields at the bottom
                ImGui::GetContentRegionAvail().y
                - openCancelSize.y * 2.0f
                - style.ItemSpacing.y * 2.0f
            );

            //  TODO: figure out what these static variables actually do,
            //        because changing them doesn't seem to have any easily
            //        discernable and consistent effect.
            static float rootsWidth = 100.0f;
            static float filesHeight = avail.y;
            Splitter(true, style.ItemSpacing.x, &rootsWidth, &filesHeight, 50.0f, 50.0f, avail.y);

            if (ImGui::BeginChild("Files", avail))
            {

//*********************
//  - - - ROOTS - - -
//*********************

                if (ImGui::BeginTable(
                    "Roots",
                    1,
                    ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter,
                    ImVec2(rootsWidth, avail.y))
                    )
                {
                    ImGui::TableSetupColumn("Roots");
                    ImGui::TableHeadersRow();
                    for (auto i : _rootDirs)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        if (ImGui::Selectable(i.string().c_str()))
                        {
                            _history.addToHistory(_currentDir);
                            _currentDir = i;
                            _selected.clear();
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }

//*********************
//  - - - FILES - - -
//*********************

            if(ImGui::BeginChild("Files"))
            {
                ImGui::SameLine();
                ImGuiTableFlags tableFileFlags =
                    ImGuiTableFlags_NoBordersInBody
                    | ImGuiTableFlags_ScrollY
                    | ImGuiTableFlags_Resizable
                    | ImGuiTableFlags_RowBg
                    | ImGuiTableFlags_BordersOuter
                    ;
                if (ImGui::BeginTable("Files", 4, tableFileFlags))
                {
                    ImGui::TableSetupColumn("Filename");
                    ImGui::TableSetupColumn("Size");
                    ImGui::TableSetupColumn("Date created");
                    ImGui::TableSetupColumn("Date modified");
                    //  Keep header visible
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableHeadersRow();
                    bool selected = false;
                    for (auto it = _dirContents.begin(); it != _dirContents.end(); ++it)
                    {
                        std::error_code ec;
                        if (std::filesystem::is_directory(it->getFilepath(), ec))
                        {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);

                            if (ImGui::Selectable(it->getFilepath().filename().string().c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns))
                            {
                                _history.addToHistory(_currentDir);
                                _currentDir = it->getFilepath();
                                _selected.clear();
                                selected = false;
                            }
                            continue;
                        }
                        else
                        {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            if (*it == _selected)
                            {
                                selected = true;
                            }
                            if (ImGui::Selectable(it->getFilepath().filename().string().c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns))
                            {
                                _selected = it->getFilepath();
                                ImGui::SetItemDefaultFocus();
                            }
                            ImGui::TableNextColumn();
                            ImGui::Text(it->getFileSizeString().c_str());

                            ImGui::TableNextColumn();
                            ImGui::Text(it->getFileCreatedDateString().c_str());

                            ImGui::TableNextColumn();
                            ImGui::Text(it->getFileLastWriteDateString().c_str());

                            selected = false;
                        }
                    }
                    ImGui::EndTable();
                }
            }
            ImGui::EndChild();

//**********************
//  - - - FILTER - - -
//**********************
            constexpr const float filterWidth = 100.0f;
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - (filterWidth));
            /*
                Options:
                    - *.all
                    - *.mpt
            */
            ImGui::PushItemWidth(filterWidth);
            if (ImGui::BeginCombo("##Filter", _filterSelection.c_str()))
            {
                if (ImGui::Selectable(DEFAULT_FILTER_SELECTION, _filterSelection == DEFAULT_FILTER_SELECTION))
                {
                    _filterSelection = DEFAULT_FILTER_SELECTION;
                    for (
                        auto extIt = ++_extensionOptions->begin();
                        extIt != _extensionOptions->end();
                        ++extIt
                        )
                    {
                        _extensionFilter.push_back(*extIt);
                    }
                    RefreshCurrentDirectory(_currentDir, _dirContents, _extensionFilter);
                }
                else
                {
                    bool selected = false;
                    for (
                        auto it = _extensionOptions->begin();
                        it != _extensionOptions->end();
                        ++it
                        )
                    {
                        if (it->getName() == _filterSelection)
                        {
                            selected = true;
                        }
                        if (ImGui::Selectable(it->getName().c_str(), selected))
                        {
                            _filterSelection = it->getName();
                            _extensionFilter.clear();
                            _extensionFilter.push_back(*it);
                            RefreshCurrentDirectory(_currentDir, _dirContents, _extensionFilter);
                        }
                        selected = false;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
            ImVec2 buttonMin = ImGui::GetItemRectMin();

//************************
//  - - - FILEPATH - - -
//************************

            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMin().x);
            constexpr const size_t pathSize = 1024;
            std::string pathName;
            pathName.reserve(pathSize);
            if (_selected.empty())
            {
                pathName = _currentDir.string();
            }
            else
            {
                pathName = _selected.getFilepath().string();
            }
            //  Calculate width to compensate for extensions dropdown
            float xWidth =
                buttonMin.x
                - ImGui::GetCursorScreenPos().x
                - style.ItemSpacing.x
                ;
            ImGui::PushItemWidth(xWidth);
            if (ImGui::InputText("##Filepath", &pathName))
            {
                if (std::filesystem::is_directory(pathName))
                {
                    _currentDir = pathName;
                    _selected.clear();
                }

                if (_purpose == MPT::FileDialogPurpose::Open)
                {
                    if (std::filesystem::is_regular_file(pathName))
                    {
                        _selected.setFilepath(pathName);
                    }
                }
                else if (_purpose == MPT::FileDialogPurpose::Save)
                {
                    if (std::filesystem::is_regular_file(pathName))
                    {
                        //  TODO: filter viable save types by extensions
                        //  TODO: warn user and ask to overwrite
                    }
                    //  TODO: resolve extensions when multiple extensions are
                    //        viable
                    else
                    {
                        //  MPT SAVE FILES ONLY!!!!
                        std::filesystem::path savePath(pathName);
                        if (savePath.extension().empty())
                        {
                            savePath.extension().replace_extension(".mpt");
                            _selected = savePath;
                        }
                        else if (savePath.extension() == ".mpt")
                        {
                            _selected = savePath;
                        }
                    }
                }
            }
            ImGui::PopItemWidth();

//***************************************
//  - - - OPEN AND CANCEL BUTTONS - - -
//***************************************
            ImGui::SetCursorPos(ImGui::GetWindowContentRegionMax() - openCancelSize);
            if (ImGui::Button("Cancel", openCancelSize))
            {
                _open = false;
            }

            ImGui::SameLine();
            ImGui::SetCursorPosX(
                ImGui::GetWindowContentRegionMax().x - (openCancelSize.x * 2.0f + style.ItemSpacing.x)
            );
            if (ImGui::Button(_buttonText.c_str(), openCancelSize))
            {
                if (_purpose == MPT::FileDialogPurpose::Open)
                {
                    if (std::filesystem::is_regular_file(_selected.getFilepath()))
                    {
                        //  Opening project file
                        app.getModel().openProject(_selected.getFilepath());
                        _open = false;
                    }
                    else
                    {
                        //  TODO: present warning to the user
                    }
                }
                else if (_purpose == MPT::FileDialogPurpose::Save)
                {
                    if (_selected.getFilepath().extension().empty())
                    {
                        _selected.getFilepath().replace_extension(".mpt");
                    }
                    else if (_selected.getFilepath().extension() != ".mpt")
                    {
                        _selected.getFilepath().replace_extension(".mpt");
                    }

                    if (std::filesystem::exists(_selected.getFilepath()))
                    {
                        ImGui::OpenPopup(PopupNames[static_cast<int>(Popup::SaveOverwrite)]);
                    }
                    else
                    {
                        app.getModel().saveAs(_selected.getFilepath());
                        _open = false;
                    }
                }
            }
            if (ImGui::IsPopupOpen(PopupNames[static_cast<int>(Popup::SaveOverwrite)]))
            {
                if (ImGui::BeginPopupModal(PopupNames[static_cast<int>(Popup::SaveOverwrite)]))
                {
                    ImGui::TextWrapped("Are you sure you want to overwrite this file?");
                    if (ImGui::Button("Yes"))
                    {
                        app.getModel().saveAs(_selected.getFilepath());
                        _open = false;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    if (startDir != _currentDir)
    {
        RefreshCurrentDirectory(_currentDir, _dirContents, _extensionFilter);
    }
}

FileDialogView& FileDialogView::operator=(const FileDialogView& dialog)
{
    _open               = dialog._open;
    _separatorX         = dialog._separatorX;
    _purpose            = dialog._purpose;
    _buttonText         = dialog._buttonText;
    _currentDir         = dialog._currentDir;
    _selected           = dialog._selected;
    _rootDirs           = dialog._rootDirs;
    _filterSelection    = dialog._filterSelection;
    _extensionOptions   = dialog._extensionOptions;

    return *this;
}

//  Taken from here:
//  https://github.com/ocornut/imgui/issues/319#issuecomment-345795629
bool Splitter(
    bool split_vertically,
    float thickness,
    float* size1,
    float* size2,
    float min_size1,
    float min_size2,
    float splitter_long_axis_size
)
{
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return ImGui::SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}
