/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "cvinvaffine.hxx"

static CvInvAffineTest suite_CvInvAffineTest;

static CxxTest::List Tests_CvInvAffineTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CvInvAffineTest( "cvinvaffine.hxx", 20, "CvInvAffineTest", suite_CvInvAffineTest, Tests_CvInvAffineTest );

static class TestDescription_CvInvAffineTest_testInvAffine : public CxxTest::RealTestDescription {
public:
 TestDescription_CvInvAffineTest_testInvAffine() : CxxTest::RealTestDescription( Tests_CvInvAffineTest, suiteDescription_CvInvAffineTest, 23, "testInvAffine" ) {}
 void runTest() { suite_CvInvAffineTest.testInvAffine(); }
} testDescription_CvInvAffineTest_testInvAffine;

#include <cxxtest/Root.cpp>
