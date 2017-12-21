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

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "main.h"
#include "servantThread.h"
#include "ums2netconfrecord.h"

/**
 * recv len bytes exactly
 *
 * @param sockfd socket file descriptor
 * @param buf the buffer
 * @param len the length of the buffer
 * @param flags the flags
 *
 * @return -1 if error. 0 or less than len is EOF
 */
ssize_t recvn(int sockfd, void *buf, size_t len, int flags) {
  ssize_t ret = 0;
  char *bufC = (char *)(buf);
  while (ret < len) {
    ssize_t r1 = recv(sockfd, &(bufC[ret]), len-((size_t)ret), flags);
    if (r1 < 0) {
      /* some error */
      int errsv = errno;
      if (errsv == EINTR) {
	continue;
      } else if (errsv == EAGAIN) {
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	select(sockfd+1, &readfds, NULL, NULL, NULL);
	continue;
      } else if (errsv == EWOULDBLOCK) {
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	select(sockfd+1, &readfds, NULL, NULL, NULL);
	continue;
      }
      if (ret == 0) {
	ret = r1;
	return ret;
      }
      char errbuf[1024];
      char *errstr;
      errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
      syslog(LOG_ERR, "recvn() error (%s)", errstr);
      break;
    } else if (r1 == 0) {
      /* EOF */
      break;
    }
    ret += (ssize_t)(r1);
  }
  return ret;
}

/**
 * Check if file exists
 *
 * @param filename the name of the file that needs to be checked
 *
 * @return 0: non-exist, 1: exist
 */
int checkFileExists(const std::string &filename) {
  int result;
  struct stat statbuf;
  memset(&statbuf, 0, sizeof(statbuf));
  result = stat(filename.c_str(), &statbuf);
  if (result < 0) {
    return 0;
  }
  return 1;
}

/**
 * the function that serves one client.
 *
 * This function will read all the data from the client and write it to the
 * device.
 *
 * @param clientSocket the socket which is connected to the client.
 * @param ddParameters the parameters for the device.
 */
void clientServant(int clientSocket, const std::map<std::string, std::string> &ddParameters) {
  char *buf=NULL;
  int bufSize = 512;
  ssize_t bufLen;
  ssize_t totalLen=0;

  /* get device path */
  std::string devFilename;
  if (ddParameters.find(std::string("of")) != ddParameters.end()) {
    devFilename = ddParameters.at(std::string("of"));
  }
  if (devFilename.length() <= 0) {
    syslog(LOG_ERR, "No device path. Please assign of=");
    return;
  }

  /* get blocksize, default: 512 */
  bufSize=512;
  if (ddParameters.find(std::string("bs")) != ddParameters.end()) {
    std::string o1 = ddParameters.at(std::string("bs"));
      std::istringstream ifs1 (o1);
      ifs1 >> bufSize;
      if (ifs1.fail()) {
	bufSize = 512;
      }
  }

  /* check if device is appeared. */
  if (!checkFileExists(devFilename)) {
    syslog(LOG_WARNING, "Device %s not appeared. Close immediately.", devFilename.c_str());
    return;
  }

  /* open the device */
  int outFD = open(devFilename.c_str(), O_RDWR);
  if (outFD < 0) {
    int errsv = errno;
    char errbuf[1024];
    char *errstr;
    errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
    syslog(LOG_ERR, "Cannot open device %s. (%s)", devFilename.c_str(), errstr);
    return;
  }

  /* allocate buffer */
  if (buf == NULL) {
    buf = (char *)malloc(sizeof(char)*bufSize);
  }
  if (buf == NULL) {
    syslog(LOG_ERR, "Malloc buffer for %d bytes failed", bufSize);
    return;
  }

  /* copy data from socket to device */
  while (!quitFlag) {
    ssize_t writeBufLen = 0;
    bufLen = recvn(clientSocket, buf, bufSize, 0);
    if (bufLen == 0) {
      syslog(LOG_DEBUG, "read from client socket ended");
      break;
    } else if (bufLen < 0) {
      int errsv = errno;
      char errbuf[1024];
      char *errstr;
      errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
      syslog(LOG_DEBUG, "read from client socket ended (%s)", errstr);
      break;
    }
    writeBufLen = write(outFD,buf,bufLen);
    if (writeBufLen < 0) {
      int errsv = errno;
      char errbuf[1024];
      char *errstr;
      errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
      syslog(LOG_DEBUG, "write to device ended (%s)", errstr);
      break;
    }
    totalLen += writeBufLen;
    if (bufLen < bufSize) {
      break;
    }
  }

  /* free the buf */
  if (buf != NULL) {
    free(buf);
    buf=NULL;
  }

  /* close output file */
  close(outFD);

  syslog(LOG_INFO, "Totally write %ld bytes to %s", totalLen, devFilename.c_str());
}

/**
 * the thread for a TCP port
 *
 * This function will bind and listen to the TCP port. If client connects,
 * it passes the client socket to clientServant() function.
 *
 * @param data the pointer of UMS2NETConfRecord instance
 *
 * @return NULL.
 */
void* servantThread(void * data) {
  UMS2NETConfRecord *record = (UMS2NETConfRecord *)(data);
  int serverSocket = -1;
  int enable=1;
  int result;

  /* create socket */
  serverSocket = socket(AF_INET6, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    int errsv = errno;
    char errbuf[1024];
    char *errstr;
    errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
    syslog(LOG_ERR, "Cannot create server socket (%s)", errstr);
    return NULL;
  }

  /* set SO_REUSEADDR to the socket */
  enable=1;
  result = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
  if (result < 0) {
    int errsv = errno;
    char errbuf[1024];
    char *errstr;
    errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
    syslog(LOG_ERR, "Cannot setsockopt() server socket (%s)", errstr);
    return NULL;
  }

  /* bind the socket to the TCP port */
  struct sockaddr_in6 serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin6_family = AF_INET6;
  serverAddr.sin6_addr = in6addr_any;
  serverAddr.sin6_port = htons(record->getPort());

  result = bind(serverSocket, (struct sockaddr *)(&serverAddr), sizeof(serverAddr));
  if (result < 0) {
    int errsv = errno;
    char errbuf[1024];
    char *errstr;
    errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
    syslog(LOG_ERR, "Cannot bind server socket to TCP port %d (%s)", record->getPort(), errstr);
    return NULL;
  }

  /* start listen to the socket */
  result = listen(serverSocket, 10);
  if (result < 0) {
    int errsv = errno;
    char errbuf[1024];
    char *errstr;
    errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
    syslog(LOG_ERR, "Cannot listen to server socket to TCP port %d (%s)", record->getPort(), errstr);
    return NULL;
  }

  /* wait for client */
  while (!quitFlag) {
    fd_set rset;
    int nReady;
    FD_ZERO(&rset);
    FD_SET(serverSocket, &rset);
    nReady = select(serverSocket+1, &rset, NULL, NULL, NULL);
    if (nReady == 1 && FD_ISSET(serverSocket, &rset)) {
      struct sockaddr_in6 clientAddr;
      socklen_t clientAddrSize = sizeof(clientAddr);
      int clientSocket = accept(serverSocket, (struct sockaddr *) (&clientAddr), &clientAddrSize);
      if (clientSocket < 0) {
	int errsv = errno;
	char errbuf[1024];
	char *errstr;
	errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
	syslog(LOG_WARNING, "Cannot accept client socket at port %d (%s)", record->getPort(), errstr);
	continue;
      }

      /* call clientServant() to move the data from the socket to device */
      clientServant(clientSocket, record->getDDParameterMap());

      /* close client socket */
      close(clientSocket);
    } else {
      int errsv = errno;
      char errbuf[1024];
      char *errstr;
      errstr = strerror_r(errsv, errbuf, sizeof(errbuf));
      syslog(LOG_WARNING, "select() on serverSocket (TCP port %d) returns bad value %d, thread loop exits. (%s)", record->getPort(), nReady, errstr);
      break;
    }
  }
  
  return NULL;
}
