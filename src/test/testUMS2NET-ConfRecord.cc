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
#include "../ums2netconfrecord.h"

class UMS2NETConfRecordTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(UMS2NETConfRecordTest);
  CPPUNIT_TEST(testGetPort);
  CPPUNIT_TEST(testGetDDParameter);
  CPPUNIT_TEST(testGetDDParameterVector);
  CPPUNIT_TEST(testGetDDParameterMap);
  CPPUNIT_TEST_SUITE_END();
  
private:
  UMS2NETConfRecord *record1;
  UMS2NETConfRecord *record2;
  UMS2NETConfRecord *record3;
  int port1 = 12345;
  std::string dd1;
  int port2 = 10000;
  std::string dd2;
  int port3 = 11111;
  std::string dd3;

public:
  void setUp() {
    port1 = 12345;
    dd1 = std::string("of=/tmp/f12345");
    port2 = 10000;
    dd2 = std::string("of=/tmp/f10000 bs=2048");
    port3 = 11111;
    dd3 = std::string("of=/tmp/f11111 bs=1024 seek=1");
    record1 = new UMS2NETConfRecord(port1, dd1);
    record2 = new UMS2NETConfRecord(port2, dd2);
    record3 = new UMS2NETConfRecord(port3, dd3);
  }

  void tearDown() {
    delete record1;
    delete record2;
    delete record3;
  }

protected:
  /**
   * test for getPort() member function
   */
  void testGetPort() {
    CPPUNIT_ASSERT_EQUAL(record1->getPort(), port1);
    CPPUNIT_ASSERT_EQUAL(record2->getPort(), port2);
    CPPUNIT_ASSERT_EQUAL(record3->getPort(), port3);
  }

  /**
   * test for getDDParameter() member function
   */
  void testGetDDParameter() {
    CPPUNIT_ASSERT(record1->getDDParameter().compare(dd1)==0);
    CPPUNIT_ASSERT(record2->getDDParameter().compare(dd2)==0);
    CPPUNIT_ASSERT(record3->getDDParameter().compare(dd3)==0);
  }

  /**
   * test for getDDParameterVector() member function
   */
  void testGetDDParameterVector() {
    std::vector<std::string> vector1 = record1->getDDParameterVector();
    std::vector<std::string> vector2 = record2->getDDParameterVector();
    std::vector<std::string> vector3 = record3->getDDParameterVector();

    CPPUNIT_ASSERT_EQUAL((int)vector1.size(), 1);
    CPPUNIT_ASSERT_EQUAL((int)vector2.size(), 2);
    CPPUNIT_ASSERT_EQUAL((int)vector3.size(), 3);

    /* when the size equals to 1, then it should be as same as whole */
    CPPUNIT_ASSERT(vector1[0].compare(dd1)==0);

    CPPUNIT_ASSERT(vector2[0].compare(std::string("of=/tmp/f10000"))==0);
    CPPUNIT_ASSERT(vector2[1].compare(std::string("bs=2048"))==0);
    CPPUNIT_ASSERT(vector3[0].compare(std::string("of=/tmp/f11111"))==0);
    CPPUNIT_ASSERT(vector3[1].compare(std::string("bs=1024"))==0);
    CPPUNIT_ASSERT(vector3[2].compare(std::string("seek=1"))==0);
  }

  /**
   * test for getDDParameterMap() member function
   */
  void testGetDDParameterMap() {
    std::map<std::string, std::string> map1 = record1->getDDParameterMap();
    std::map<std::string, std::string> map2 = record2->getDDParameterMap();
    std::map<std::string, std::string> map3 = record3->getDDParameterMap();
    
    CPPUNIT_ASSERT_EQUAL((int)map1.size(), 1);
    CPPUNIT_ASSERT_EQUAL((int)map2.size(), 2);
    CPPUNIT_ASSERT_EQUAL((int)map3.size(), 3);
    CPPUNIT_ASSERT(map1.at(std::string("of")).compare(std::string("/tmp/f12345"))==0);
    CPPUNIT_ASSERT(map2.at(std::string("of")).compare(std::string("/tmp/f10000"))==0);
    CPPUNIT_ASSERT(map2.at(std::string("bs")).compare(std::string("2048"))==0);
    CPPUNIT_ASSERT(map3.at(std::string("of")).compare(std::string("/tmp/f11111"))==0);
    CPPUNIT_ASSERT(map3.at(std::string("bs")).compare(std::string("1024"))==0);
    CPPUNIT_ASSERT(map3.at(std::string("seek")).compare(std::string("1"))==0);
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(UMS2NETConfRecordTest);

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
