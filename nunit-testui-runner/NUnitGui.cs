// ****************************************************************
// This is free software licensed under the NUnit license. You
// may obtain a copy of the license as well as information regarding
// copyright ownership at http://nunit.org.
// ****************************************************************

using System.Diagnostics;

namespace NUnit.ConsoleRunner
{
	using System;
	using System.IO;
	using System.Reflection;
	using System.Xml;
	using System.Resources;
	using System.Text;
	using NUnit.Core;
	using NUnit.Core.Filters;
	using NUnit.Util;


    /// <summary>
    /// Summary description for GuiEventCollector.
    /// </summary>
    public class GuiEventCollector : MarshalByRefObject, EventListener
    {
        public static string ElapsedValue(double d)
        {
            return Math.Round(1000 * d).ToString();
        }

        public static string BoolValue(bool b)
        {
            return b ? "1" : "0";
        }

        public GuiEventCollector()
        {
            AppDomain.CurrentDomain.UnhandledException +=
                new UnhandledExceptionEventHandler(OnUnhandledException);
        }

        public void RunStarted(string name, int testCount)
        {
            Console.WriteLine("#RunStarted {0}", testCount);
        }

        public void RunFinished(TestResult result)
        {
            Console.WriteLine("#RunFinished {0}", ElapsedValue(result.Time));
        }

        public void RunFinished(Exception exception)
        {
            UnhandledException(exception);
            Console.WriteLine("#RunFinished");
        }

        public void TestFinished(TestResult testResult)
        {
            if (!testResult.IsSuccess)
            {
                Console.WriteLine("{0}: {1}", testResult.FullName, testResult.ResultState);
			    string stackTrace = StackTraceFilter.Filter(testResult.StackTrace);
			    if (stackTrace != null && stackTrace != string.Empty)
                    Console.WriteLine(stackTrace);
                Console.WriteLine(testResult.Message);
            }
            Console.WriteLine("#TestFinished {0} {1} {2}", testResult.Test.TestName.TestID, ElapsedValue(testResult.Time), BoolValue(testResult.IsSuccess));
        }

        public void TestStarted(TestName testName)
        {
            Console.WriteLine("#TestStarted {0}", testName.TestID);
        }

        public void SuiteStarted(TestName testName)
        {
            Console.WriteLine("#SuiteStarted {0}", testName.TestID);
        }

        public void SuiteFinished(TestResult suiteResult)
        {
            Console.WriteLine("#SuiteFinished {0} {1}", suiteResult.Test.TestName.TestID, ElapsedValue(suiteResult.Time));
        }

        private void OnUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            if (e.ExceptionObject.GetType() != typeof(System.Threading.ThreadAbortException))
            {
                this.UnhandledException((Exception)e.ExceptionObject);
            }
        }

        public void UnhandledException(Exception exception)
        {
            Console.WriteLine("#Exception {0}", exception.ToString());
        }

        public void TestOutput(TestOutput output)
        {
            Console.Write(output.Text);
        }

        public override object InitializeLifetimeService()
        {
            return null;
        }
    }

    /// <summary>
    /// Summary description for Gui.
    /// </summary>
    public class Gui
    {
        public static void List(ITest suite)
        {
            List(suite, "");
        }

		private static string Label(ITest test)
		{
			char type;
			if (test.IsSuite)
				type = (test.RunState == RunState.Explicit) ? 's' : 'S';
			else
				type = (test.RunState == RunState.Explicit) ? 'c' : 'C';

			return string.Format("{0}{1}:{2}", type, test.TestName.TestID, test.TestName.Name);
		}

        public static void List(ITest suite, string indent)
        {
            foreach (ITest test in suite.Tests)
            {
				if (test.RunState != RunState.Ignored)
				{
					Console.WriteLine(indent + Label(test));
					if (test.IsSuite)
						List(test, indent + "    ");
				}
			}
        }

        public static void Wait()
        {
            Console.WriteLine("#Waiting");
            Console.ReadLine();
        }

		public static bool CommandCompleted(ConsoleOptions options, TestRunner testRunner, TestFilter testFilter)
		{
			if (options.gui_list)
			{
				Gui.List(testRunner.Test);
				testRunner.Unload();
				return true;
			}

			if (options.gui_wait)
			{
				Gui.Wait();
			}

			if (options.gui_run)
			{
				testRunner.Run(new GuiEventCollector(), testFilter);
				return true;
			}

			return false;
		}
    }
}
