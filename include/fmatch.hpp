#pragma once

#include <string>
#include <vector>

namespace fmatch {

    inline char pathSeparator()
    {
        #if defined(_WIN32)
            return '\\';
        #elif defined(unix) || defined(__MACH__)
            return '/';
        #endif

        return '\0';
    }

    inline bool isPathSeparator(char ch, bool any_separator = false)
    {
        if(any_separator && (ch == '\\' || ch == '/')) {
            return true;
        }

        return ch == pathSeparator();
    }

    inline std::string normalizePath(const std::string& str)
    {
        std::string s;

        if(str.empty()) {
            return s;
        }

        for(int i = 0; i < str.size(); i++) {
            if(isPathSeparator(str[i], true)) {
                s.push_back(pathSeparator());
                while(isPathSeparator(str[i], true)) {
                    i++;
                }
                i--;
                continue;
            }
            s.push_back(str[i]);
        }

        while(!s.empty() && s.back() == pathSeparator()) {
            s.pop_back();
        }

        return s;
    }

    inline std::vector<std::string> separatePaths(const std::string& str)
    {
        std::vector<std::string> result;
        std::string temp;
        for(int i = 0; i < str.size(); i++) {
            if(isPathSeparator(str[i], true)) {
                result.push_back(temp);
                temp.clear();
                while(i < str.size() && isPathSeparator(str[i], true)) i++;
                i--;
                continue;
            }

            if(i >= str.size()-1) {
                temp.push_back(str[i]);
                result.push_back(temp);
                temp.clear();
                continue;
            }

            temp.push_back(str[i]);
        }

        return result;
    }

    inline bool match(const std::string& str, const std::string& pattern)
    {
        std::vector<std::string> str_list = separatePaths(str);
        std::vector<std::string> pattern_list = separatePaths(pattern);

        int i = 0;
        int j = 0;
        while(i < str_list.size() && j < pattern_list.size()) {
            // str_list_new_pos = str_list_size - (pattern_list_size - double_asterisk_pos - 1)
            if(pattern_list[j] == "**") {
                i = str_list.size() - (pattern_list.size() - j - 1);
                j++;
                continue;
            }
            
            int k = 0; // Iterator for str_list[i]
            int l = 0; // Iterator for pattern_list[j]
            const std::string& s = str_list[i];
            const std::string& p = pattern_list[j];
            bool matched = false;
            while(k < s.size() && l < p.size()) {
                if(p[l] == '*') {
                    l++;

                    if(l >= p.size()) {
                        matched = true;
                        break;
                    }

                    while(k < s.size() && s[k] != p[l]) k++;

                    if(k >= s.size()) return false;
                    
                } else if(p[l] != '?' && s[k] != p[l]) {
                    return false;
                } else {
                    k++;
                    l++;
                }
            }

            if(p.back() == '*') {
                matched = true;
            }

            if((k < s.size() || l < p.size()) && !matched) return false;

            i++;
            j++;
        }

        if(i < str_list.size() || j < pattern_list.size()) return false;

        return true;
    }
}