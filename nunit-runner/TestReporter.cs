//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    enum LogLevel { Minimal, Medium, All };

    enum TestCaseState { Ignored, Failed, Success };

    class TestReporter
    {

        LogLevel logLevel;

        public TestReporter(LogLevel logLevel)
        {
            this.logLevel = logLevel;
        }

        private static int BoolAsInt(bool b)
        {
            return b ? 1 : 0;
        }

        private static string TestCaseStateToString(TestCaseState state)
        {
            switch (state)
            {
                case TestCaseState.Ignored: return "-";
                case TestCaseState.Failed: return "0";
                case TestCaseState.Success: return "1";
            }
            return "";
        }

        public void BeginRun()
        {
            System.Console.WriteLine("#RunStarted {0}", 0);
        }

        public void BeginSuite(TestItem suite)
        {
            System.Console.WriteLine("#SuiteStarted {0}", suite.Id);
            if (logLevel == LogLevel.All)
                System.Console.WriteLine("Entering test suite {0}", suite.Name);
        }

        public void BeginTest(TestItem test)
        {
            System.Console.WriteLine("#TestStarted {0}", test.Id);
            if (logLevel == LogLevel.All)
                System.Console.WriteLine("Entering test case {0}", test.Name);
        }

        public void Ignore(string message)
        {
            if (logLevel != LogLevel.Minimal)
                System.Console.WriteLine("Ignored: \"{0}\"", message);
        }

        public void Pass(string message)
        {
            System.Console.WriteLine("Assert.Pass: \"{0}\"", message);
        }

        public void Exception(string message)
        {
            System.Console.WriteLine("#BeginException");
            System.Console.WriteLine("Exception: {0}", message);
            System.Console.WriteLine("#EndException");
        }

        public void EndTest(TestItem test, int ms, TestCaseState state)
        {
            if (logLevel == LogLevel.All)
                System.Console.WriteLine("Leaving test case {0}", test.Name);
            System.Console.WriteLine("#TestFinished {0} {1} {2}", test.Id, ms, TestCaseStateToString(state));
        }

        public void EndSuite(TestItem suite, int ms)
        {
            if (logLevel == LogLevel.All)
                System.Console.WriteLine("Leaving test suite {0}", suite.Name);
            System.Console.WriteLine("#SuiteFinished {0} {1}", suite.Id, ms);
        }

        public void EndRun()
        {
            System.Console.WriteLine("#RunFinished {0}", 0);
        }
    };
}
