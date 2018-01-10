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
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include "../configReader.h"

class UMS2NETConfigReaderTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(UMS2NETConfigReaderTest);
  CPPUNIT_TEST(testConfigReader);
  CPPUNIT_TEST_SUITE_END();
  
private:
  char *filename;
  int fd;

public:
  void setUp() {
    const char * data[] = {
      "# test ums2net config\n",
      "12345 of=/tmp/f12345\n",
      "10000 of=/tmp/f10000 bs=2048\n"
      "11111 of=/tmp/f11111 bs=4096 seek=2\n" };

    filename = strdup("ums2net-testUMS2NET-ConfigReader-XXXXXX");
    fd = mkstemp(filename);
    for (int i=0; i<sizeof(data)/sizeof(data[0]); i++) {
      ssize_t w1;
      w1 = write(fd, data[i], strlen(data[i]));
      if (w1 == -1) {
	int errsv = errno;
	fprintf(stderr, "Error write to file (%s)", strerror(errsv));
	break;
      }
    }
    sync();
  }

  void tearDown() {
    close(fd);
    unlink(filename);
    free(filename);
  }

protected:
  /**
   * test for getConfig() function
   */
  void testConfigReader() {
    CPPUNIT_ASSERT(fd >= 0);
    std::vector<UMS2NETConfRecord> results;
    results = getConfig(filename);
    CPPUNIT_ASSERT_EQUAL((int)results.size(), 3);
    CPPUNIT_ASSERT_EQUAL(results[0].getPort(), 12345);
    CPPUNIT_ASSERT_EQUAL(results[1].getPort(), 10000);
    CPPUNIT_ASSERT_EQUAL(results[2].getPort(), 11111);
    CPPUNIT_ASSERT(results[0].getDDParameter().compare(std::string("of=/tmp/f12345"))==0);
    CPPUNIT_ASSERT(results[1].getDDParameter().compare(std::string("of=/tmp/f10000 bs=2048"))==0);
    CPPUNIT_ASSERT(results[2].getDDParameter().compare(std::string("of=/tmp/f11111 bs=4096 seek=2"))==0);
      
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(UMS2NETConfigReaderTest);

int main(int argc, char* argv[]) {
    // informs test-listener about testresults
    CPPUNIT_NS::TestResult testresult;

    // register listener for collecting the test-results
    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);

    // register listener for per-test progress output
    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener (&progress);

    // insert test-suite at test-runner by registry
    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest ());
    testrunner.run(testresult);

    // output results in compiler-format
    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write ();
 
    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}
