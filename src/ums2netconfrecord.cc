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

#include <string>
#include <sstream>
#include "ums2netconfrecord.h"

/**
 * UMS2NETConfRecord constructor
 *
 * @param port the TCP port
 * @param ddParameter the string of the parameter in dd operand format.
 */ 
UMS2NETConfRecord::UMS2NETConfRecord(int port, std::string &ddParameter) : port(port), ddParameter(ddParameter) {
}

/**
 * get TCP port number
 *
 * @return the TCP port number
 */
int UMS2NETConfRecord::getPort() const {
  return port;
}

/**
 * get parameter string
 *
 * @return the parameter string
 */
std::string UMS2NETConfRecord::getDDParameter() const {
  return ddParameter;
}

/**
 * get parameter vector
 *
 * the parameter string is parsed and will split into vector
 *
 * @return the vector of parameters
 */
std::vector<std::string> UMS2NETConfRecord::getDDParameterVector() const {
  std::vector<std::string> ret;
  std::istringstream iss(ddParameter);
  std::string token;
  while (std::getline(iss, token, ' ')) {
    ret.push_back(token);
  }
  return ret;
}

/**
 * get parameter map
 *
 * the parameter string is parsed and will split into map (key, value)
 *
 * @return the map of parameters
 */
std::map<std::string, std::string> UMS2NETConfRecord::getDDParameterMap() const {
  std::map<std::string, std::string> ret;
  std::vector<std::string> parametersV = getDDParameterVector();
  std::string delimEqual("=");
  for (int i=0; i<(int)parametersV.size(); i++) {
    std::size_t found = parametersV[i].find(delimEqual);
    if (found == std::string::npos) {
      continue;
    }
    std::string k1 = parametersV[i].substr(0,found);
    std::string v1 = parametersV[i].substr(found+1);
    if (k1.length() <= 0) {
      continue;
    }
    ret[k1] = v1;
  }
  return ret;
}
