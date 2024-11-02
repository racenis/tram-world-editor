#ifndef EDITOR_LANGUAGE_H
#define EDITOR_LANGUAGE_H

#include <string>

namespace Editor {
    void ResetLanguage();
    std::wstring Get(const std::string& name);
}

#endif // EDITOR_LANGUAGE_H