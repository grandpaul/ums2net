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
#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <syslog.h>

#include "main.h"
#include "configReader.h"
#include "servantThread.h"

int quitFlag=0;

/**
 * print usage
 *
 * @param prog the name of the program
 *
 * @return always 0
 */
int usage(const char *prog) {
  std::cerr << "Usage: " << prog << " -c <configFile>" << std::endl;
  return 0;
}

std::vector<pthread_t> threads;

/**
 * start threads for each TCP port.
 *
 * @param records the config records
 */
void startServantThreads(const std::vector<UMS2NETConfRecord> &records) {
  for (int i=0; i<(int)(records.size()); i++) {
    pthread_t thread1;
    pthread_create(&thread1, NULL, servantThread, (void *)(&(records[i])));
    threads.push_back(thread1);
  }
}

/**
 * join threads.
 *
 * @param records the config records
 */
void joinServantThreads() {
  for (int i=((int)(threads.size()))-1; i>=0; i--) {
    void *result1 = NULL;
    int result;
    pthread_t thread1 = threads[i];
    result = pthread_join(thread1, &result1);
    if (result < 0) {
      int errsv = errno;
      char errbuf[1024];
      char *errstr;
      errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
      syslog(LOG_DEBUG, "fail to join thread (%s)", errstr);
    }
    threads.pop_back();
  }
}


int main(int argc, char **argv) {
  std::string configFilename;
  int opt;

  while ((opt = getopt(argc, argv, "c:")) != -1) {
    switch(opt) {
    case 'c':
      configFilename = std::string(optarg);
      break;
    default:
      usage(argv[0]);
      exit(1);
    }
  }
  if (configFilename.length() == 0) {
    usage(argv[0]);
    exit(1);
  }
  std::vector<UMS2NETConfRecord> confRecords = getConfig(configFilename);
  if (confRecords.size() <= 0) {
    syslog(LOG_WARNING, "No activate config. Quit immediately");
    exit(0);
  }
  startServantThreads(confRecords);
  joinServantThreads();
  return 0;
}
