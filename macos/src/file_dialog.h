#ifndef APPLE_FILE_DIALOG_H
#define APPLE_FILE_DIALOG_H

#include <string>
#include <vector>

std::vector<std::string>openFileDialog(char const * const aTitle, char const * const aDefaultPathAndFile, const std::vector<std::string> & filters);

#endif
