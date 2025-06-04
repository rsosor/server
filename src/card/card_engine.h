#pragma once

#include <vector>
#include <string>

bool is_single(const std::vector<std::string>& cards);
bool is_pair(const std::vector<std::string>& cards);
bool beats(const std::vector<std::string>& new_play, const std::vector<std::string>& old_play);
