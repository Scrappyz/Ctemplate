#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace format {

    class Table {
        private:
            std::vector<std::vector<std::string>> table_;
            char row_separator_;
            char column_separator_;
            int column_space_;

            std::vector<std::vector<std::string>> cleanTableValues(const std::vector<std::vector<std::string>>& table_values) const
            {
                /*
                    Add new rows based on the max number of endlines found in the row
                    Eg:
                    {
                        {"Apple", "Banana\nNana\nBan", "Cherry\nHenry"},
                        {"Grapes", "Orange", "Pineapple"}
                    }
                    to
                    {
                        {"", "", ""},
                        {"", "", ""},
                        {"", "", ""},
                        {"", "", ""}
                    }

                    In this case, the max number of '\n' in the first row is 2. 
                    So we add 2 more rows.
                */
                std::vector<std::vector<std::string>> cleaned_table_values(table_values.size(), std::vector<std::string>(table_values[0].size(), ""));
                for(int i = 0; i < table_values.size(); i++) {
                    int max_endlines = 0;
                    for(int j = 0; j < table_values[i].size(); j++) {
                        int endlines = 0;

                        for(int k = 0; k < table_values[i][j].size(); k++) {
                            char ch = table_values[i][j][k];

                            if(ch == '\n') {
                                endlines++;
                            }
                        }

                        max_endlines = std::max(max_endlines, endlines);
                    }

                    for(int j = 0; j < max_endlines; j++) {
                        cleaned_table_values.push_back(std::vector<std::string>(table_values[i].size(), ""));
                    }
                }

                /*
                    Put the strings next to the newline to the next row.
                    Eg:
                    {
                        {"Apple", "Banana\nNana\nBan", "Cherry\nHenry"},
                        {"Grapes", "Orange", "Pineapple"}
                    }
                    to
                    {
                        {"Apple", "Banana", "Cherry"},
                        {"", "Nana", "Henry"},
                        {"", "Ban", ""},
                        {"Grapes", "Orange", "Pineapple"}
                    }
                */
                int temp_i = 0;
                for(int i = 0; i < table_values.size(); i++) {
                    int max_endlines = 0;

                    for(int j = 0; j < table_values[i].size(); j++) {
                        int endlines = 0;

                        for(int k = 0; k < table_values[i][j].size(); k++) {
                            char ch = table_values[i][j][k];

                            if(ch == '\n') {
                                endlines++;
                                continue;
                            }

                            cleaned_table_values[temp_i + endlines][j].push_back(ch);
                        }

                        max_endlines = std::max(max_endlines, endlines);
                    }

                    temp_i += max_endlines + 1;
                }

                return cleaned_table_values;
            }

            std::vector<int> maxColumnWidths(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& table_values) const
            {
                // Determine the maximum width for all rows
                std::vector<int> col_widths;

                if(!headers.empty()) {
                    col_widths = std::vector<int>(headers.size(), 0);
                } else if(!table_values.empty()) {
                    col_widths = std::vector<int>(table_values[0].size(), 0);
                } else {
                    return col_widths;
                }

                std::vector<std::vector<std::string>> combined;
                combined.push_back(headers);
                combined.insert(combined.end(), table_values.begin(), table_values.end());

                for(int i = 0; i < combined.size(); i++) {
                    for(int j = 0; j < combined[i].size(); j++) {
                        if(combined[i][j].size() > col_widths[j]) {
                            col_widths[j] = combined[i][j].size();
                        }
                    }
                }

                return col_widths;
            }

            std::vector<int> maxColumnWidths(const std::vector<std::vector<std::string>>& table_values) const
            {
                return maxColumnWidths({}, table_values);
            }
        
        public:
            // Constructors
            Table() : table_(), row_separator_(), column_separator_(), column_space_() {}
            Table(const std::vector<std::vector<std::string>>& table_values, int space = 1) : table_(table_values),
              row_separator_(), column_separator_(), column_space_(space) {}
            
            Table(const std::vector<std::vector<std::string>>& table_values, char row_separator,
                  char column_separator, int space)
            : table_(table_values), row_separator_(row_separator), column_separator_(column_separator), column_space_(space) {}

            // Setters
            void setTableValues(const std::vector<std::vector<std::string>>& table_values)
            {
                table_ = table_values;
            }

            void setRowSeparator(char row_separator_char)
            {
                row_separator_ = row_separator_char;
            }

            void setColumnSeparator(char column_separator_char)
            {
                column_separator_ = column_separator_char;
            }

            void setColumnSpace(int space)
            {
                column_space_ = space;
            }

            // Modifiers
            void appendTableValue(const std::vector<std::string>& value)
            {
                table_.push_back(value);
            }

            void appendTableValue(const std::vector<std::vector<std::string>>& values)
            {
                table_.insert(table_.end(), values.begin(), values.end());
            }

            void popTableValue()
            {
                table_.pop_back();
            }

            void insertTableValueAt(const std::vector<std::string>& value, int index)
            {
                table_.insert(table_.begin() + index, value);
            }

            void insertTableValueAt(const std::vector<std::vector<std::string>>& values, int index)
            {
                table_.insert(table_.begin() + index, values.begin(), values.end());
            }

            std::vector<std::string>& operator[](int index)
            {
                return table_[index];
            }

            // Getters
            std::vector<std::vector<std::string>> getTable() const
            {
                return table_;
            }

            std::string toString() const
            {
                std::string table_str;
                std::vector<std::vector<std::string>> cleaned_table_values = cleanTableValues(table_);
                std::vector<int> col_widths = maxColumnWidths(cleaned_table_values);

                if(cleaned_table_values.empty()) {
                    return table_str;
                }

                std::string column_sep = std::string(column_space_, ' ');
                if(column_separator_ != '\0') {
                    if(column_sep.size() == 1) {
                        column_sep[0] = column_separator_;
                    } else {
                        column_sep[column_sep.size() / 2] = column_separator_;
                    }
                }

                // Subtract the max width with the size of the string to get the correct amount of space
                for(int i = 0; i < cleaned_table_values.size(); i++) {
                    for(int j = 0; j < cleaned_table_values[i].size(); j++) {
                        std::string value = cleaned_table_values[i][j];
                        table_str.append(value);
                        if(j < cleaned_table_values[i].size()-1) {
                            table_str.append(std::string(col_widths[j] - value.size(), ' '));
                            table_str.append(column_sep);
                        }
                    }

                    table_str.push_back('\n');

                    if(row_separator_ == '\0' || i > 0) {
                        continue;
                    }

                    for(int j = 0; j < col_widths.size(); j++) {
                        table_str.append(std::string(col_widths[j], row_separator_));
                        if(j < col_widths.size()-1) {
                            table_str.append(column_sep);
                        }
                    }

                    table_str.push_back('\n');
                }

                return table_str;
            }

            void print() const
            {
                std::cout << toString();
            }
    };
}