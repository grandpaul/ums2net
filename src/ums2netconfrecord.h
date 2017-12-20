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

#ifndef _HEADER_UMS2NETCONFRECORD_HEAD1_H
#define _HEADER_UMS2NETCONFRECORD_HEAD1_H

#include <string>
#include <vector>
#include <map>

/**
 * This class stores one line of config file. This class is basically
 * read-only after construction.
 */
class UMS2NETConfRecord {
 private:
  int port; ///< TCP port
  std::string ddParameter; ///< parameters in dd operand format.

 public:
  UMS2NETConfRecord(int, std::string &);
  int getPort() const;
  std::string getDDParameter() const;
  std::vector<std::string> getDDParameterVector() const;
  std::map<std::string, std::string> getDDParameterMap() const;
};

#endif /* _HEADER_UMS2NETCONFRECORD_HEAD1_H */
