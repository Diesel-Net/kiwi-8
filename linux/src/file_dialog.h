#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <vector>
#include <string>

std::vector<std::string> openFileDialog(const std::string &title, const std::string &defaultDir, const std::vector<std::string> &filterExtensions);

#endif
