/*
 *  Copyright (C) 2017 Linaro
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "configReader.h"

/**
 * read config from file.
 *
 * @param filename the file name of the config file.
 *
 * @return the vector of UMS2NETConfRecord represents the config.
 */
std::vector<UMS2NETConfRecord> getConfig(const std::string &filename) {
  std::vector<UMS2NETConfRecord> ret;
  std::fstream file;
  std::string delimSpace(" ");
  file.open(filename, std::ios::in);

  std::string line;
  while (std::getline(file, line)) {
    if (line[0] == '#' || line[0] == ';') {
      continue;
    }
    std::size_t found = line.find(delimSpace);
    if (found == std::string::npos) {
      continue;
    }
    std::string portS = line.substr(0,found);
    std::string ddS = line.substr(found+1);
    std::istringstream istr(portS);
    int port;
    istr >> port;
    if (istr.fail()) {
      continue;
    }
    UMS2NETConfRecord r1 = UMS2NETConfRecord(port, ddS);
    ret.push_back(r1);
  }
  return ret;
}
