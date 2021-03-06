#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace Tools {
// Trims a string of the character c
std::string& Trim(std::string& str, char c);
double ReadDouble(std::istringstream& iss);
int ReadInt(std::istringstream& iss);
std::string ReadString(std::istringstream& iss);
bool FileExists(std::string filename);
std::string ToString(double d);
std::string ToString(unsigned int i);
std::vector<std::string> Split(std::string s, char on);
std::string ToLower(std::string s);
std::vector<std::map<std::string, std::string>> Filter(
    const std::vector<std::map<std::string, std::string>>&,
    std::function<bool(const std::map<std::string, std::string>&)>);
}

#endif
