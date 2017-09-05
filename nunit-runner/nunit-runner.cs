//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    using System.Linq;

    class Program
    {
        static bool list = false;
        static LogLevel logLevel = LogLevel.All;
        static bool wait = false;
        static bool run = false;
        static int randomize = -1;
        static TestCaseFilter filter = null;

        static LogLevel GetLogLevel(string arg)
        {
            switch (arg)
            {
                case "minimal": return LogLevel.Minimal;
                case "medium": return LogLevel.Medium;
                case "all":
                default: return LogLevel.All;
            }
        }

        static void Help()
        {
            System.Console.WriteLine(
                "nunit_runner runs NUnit processes for BoostTestUi. Use BoostTestUi to run tests."
            );
        }

        static void HandleOption(string arg)
        {
            int i = arg.IndexOfAny(":=".ToCharArray());
            string value = null;
            if (i > 0)
            {
                value = arg.Substring(i + 1);
                arg = arg.Substring(0, i);
            }

            if (arg == "?" || arg.ToLower() == "h")
            {
                Help();
            }
            if (arg.ToLower() == "list")
            {
                list = true;
            }
            else if (arg.ToLower() == "log")
            {
                logLevel = GetLogLevel(value);
            }
            else if (arg.ToLower() == "wait")
            {
                wait = true;
            }
            else if (arg.ToLower() == "randomize")
            {
                randomize = (value != null) ? int.Parse(value) : System.Environment.TickCount;
            }
            else if (arg.ToLower() == "run")
            {
                run = true;
                filter = (value != null) ? new TestCaseNameFilter(value) : new TestCaseDefaultFilter() as TestCaseFilter;
            }
        }

        static System.Collections.Generic.List<string> ReadOptions(string[] args)
        {
            var libs = new System.Collections.Generic.List<string>();
            if (args.Length == 0)
            {
                Help();
            }
            foreach (string arg in args)
            {
                if (arg.StartsWith("/"))
                    HandleOption(arg.Substring(1));
                else
                    libs.Add(arg);
            }
            return libs;
        }

        static void Wait()
        {
            System.Console.WriteLine("#Waiting");
            System.Console.ReadLine();
        }

        static void Main(string[] args)
        {
            try
            {
                var libs = ReadOptions(args);
                foreach (string lib in libs)
                {
                    var filename = System.IO.Path.GetFullPath(lib);
                    System.AppDomain.CurrentDomain.SetData("APPBASE", System.IO.Path.GetDirectoryName(filename));
                    using (AssemblyResolver resolver = new AssemblyResolver(System.IO.Path.GetDirectoryName(filename)))
                    {
                        TestLib testLib = new TestLib(filename, randomize);
                        if (list)
                            testLib.List();
                        if (wait)
                            Wait();
                        if (run)
                            testLib.Run(filter, new TestReporter(logLevel));
                    }
                }
            }
            catch (System.Exception ex)
            {
                System.Console.Error.WriteLine("Error: " + ex);
            }
        }
    }
}
