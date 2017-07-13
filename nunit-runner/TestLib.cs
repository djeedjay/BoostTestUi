//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    using System.Linq;
    class TestLib
    {
        TestNamespace global;
        System.Random random = null;

        private string ArgumentString(object arg)
        {
            if (arg.GetType() == typeof(string))
                return "\"" + arg.ToString() + "\"";
            return arg.ToString();
        }

        private string ArgumentString(object[] arguments)
        {
            string s = "";
            foreach (var arg in arguments)
            {
                if (s.Length > 0)
                    s += ", ";
                s += ArgumentString(arg);
            }
            return s;
        }

        private static int CompareTypesByName(System.Type x, System.Type y)
        {
            return x.Name.CompareTo(y.Name);
        }

        public TestLib(string name, int randomSeed)
        {
            global = new TestNamespace("Global");
            random = (randomSeed < 0) ? null : new System.Random(randomSeed);
            if (random != null)
                System.Console.WriteLine("Test cases order is shuffled using seed: {0}", randomSeed);

            var lib = System.Reflection.Assembly.LoadFile(name);
            System.Type[] classes = lib.GetTypes();
            System.Array.Sort(classes, CompareTypesByName);
            foreach (System.Type type in classes)
            {
                if (type.IsClass)
                {
                    var attrs = Reflection.GetAttributes(type, typeof(NUnit.Framework.TestFixtureAttribute), false);
                    if (attrs.Count() == 1)
                    {
                        global.Add(type.FullName, type, null);
                    }
                    else
                    {
                        foreach (var attr in attrs)
                        {
                            var arguments = Reflection.GetProperty(attr, "Arguments") as object[];
                            global.Add(type.FullName + "." + MethodHelper.GetDisplayName(type.GetConstructor(MethodHelper.GetArgumentTypes(arguments)), arguments), type, arguments);
                        }
                    }
                }
            }
        }

        private static string Label(Test test, TestCase testCase)
        {
            char type = test.HasExplicitAttribute || test.HasIgnoreAttribute ? 'c' : 'C';
            return string.Format("{0}{1}:[]{2}", type, testCase.Id, testCase.Name);
        }

        private static string Label(Test test)
        {
            var categories = string.Join(",", test.Categories);
            string type = test.TestCases.Count == 0 ? "C" : "S";
            if (test.HasExplicitAttribute || test.HasIgnoreAttribute)
                type = type.ToLower();
            return string.Format("{0}{1}:[{2}]{3}", type, test.Id, categories, test.Name);
        }

        private static string Label(TestFixtureClass fixture)
        {
            var categories = string.Join(",", fixture.Categories);
            char type = fixture.HasExplicitAttribute || fixture.HasIgnoreAttribute ? 's' : 'S';
            return string.Format("{0}{1}:[{2}]{3}", type, fixture.Id, categories, fixture.Name);
        }

        private static string Label(TestNamespace ns)
        {
            return string.Format("S{0}:[]{1}", ns.Id, ns.Name);
        }

        public void List()
        {
            foreach (var item in global.Items)
            {
                if (item is TestNamespace)
                    List("", item as TestNamespace);
                if (item is TestFixtureClass)
                    List("", item as TestFixtureClass);
            }
        }

        static private void List(string indent, TestNamespace ns)
        {
            System.Console.WriteLine(indent + Label(ns));
            foreach (var item in ns.Items)
            {
                if (item is TestNamespace)
                    List(indent + "  ", item as TestNamespace);
                if (item is TestFixtureClass)
                    List(indent + "  ", item as TestFixtureClass);
            }
        }

        static private void List(string indent, TestFixtureClass fixture)
        {
            System.Console.WriteLine(indent + Label(fixture));
            foreach (var test in fixture.Tests)
            {
                List(indent + "  ", test);
            }
        }

        static private void List(string indent, Test test)
        {
            System.Console.WriteLine(indent + Label(test));
            foreach (var testCase in test.TestCases)
            {
                List(indent + "  ", test, testCase);
            }
        }

        static private void List(string indent, Test test, TestCase testCase)
        {
            System.Console.WriteLine(indent + Label(test, testCase));
        }

        public void Shuffle<T>(System.Collections.Generic.IList<T> list)
        {
            int n = list.Count;
            while (n > 1)
            {
                int k = random.Next(n);
                --n;
                var t = list[k];
                list[k] = list[n];
                list[n] = t;
            }
        }

        public void Run(TestCaseFilter filter, TestReporter reporter)
        {
            reporter.BeginRun();
            Run("", global, filter, reporter);
            reporter.EndRun();
        }

        public System.Collections.Generic.IList<T> GetItemOrder<T>(System.Collections.Generic.IList<T> items)
        {
            if (random == null)
                return items;

            var list = new System.Collections.Generic.List<T>(items);
            Shuffle(list);
            return list;
        }

        private void Run(string path, TestNamespace ns, TestCaseFilter filter, TestReporter reporter)
        {
            reporter.BeginSuite(ns);
            foreach (var item in GetItemOrder(ns.Items))
            {
                string itemPath = path;
                if (itemPath != "")
                    itemPath += '.';
                itemPath += item.Name;
                if (filter.Match(itemPath))
                {
                    if (item is TestNamespace)
                        Run(itemPath, item as TestNamespace, filter, reporter);
                    if (item is TestFixtureClass)
                        Run(itemPath, item as TestFixtureClass, filter, reporter);
                }
            }
            reporter.EndSuite(ns, 0);
        }

        static private void TestFixtureTearDown(TestFixture fixture, TestReporter reporter)
        {
            try
            {
                fixture.TestFixtureTearDown();
            }
            catch (System.Exception e)
            {
                Exception.Report(e, reporter);
            }
        }

        private void Run(string path, TestFixtureClass fixtureClass, TestCaseFilter filter, TestReporter reporter)
        {
            reporter.BeginSuite(fixtureClass);
            if (fixtureClass.HasIgnoreAttribute)
            {
                reporter.Ignore(fixtureClass.IgnoreMessage);
                reporter.EndSuite(fixtureClass, 0);
                return;
            }

            TestFixture fixture = null;
            bool setupDone = false;
            try
            {
                fixture = fixtureClass.CreateInstance();
                fixture.TestFixtureSetUp();
                setupDone = true;
            }
            catch (System.Exception e)
            {
                Exception.Report(e, reporter);
            }

            foreach (var test in GetItemOrder(fixtureClass.Tests))
            {
                var testPath = path + '.' + test.Name;
                if (filter.Match(testPath))
                    Run(setupDone, testPath, fixture, test, filter, reporter);
            }

            if (setupDone)
                TestFixtureTearDown(fixture, reporter);

            reporter.EndSuite(fixtureClass, 0);
        }

        private void Run(bool ok, string path, TestFixture fixture, Test test, TestCaseFilter filter, TestReporter reporter)
        {
            if (test.TestCases.Count > 0)
            {
                reporter.BeginSuite(test);
                foreach (var testCase in GetItemOrder(test.TestCases))
                {
                    if (filter.Match(path + "." + testCase.Name))
                        Run(ok, fixture, test, testCase, reporter);
                }
                reporter.EndSuite(test, 0);
                return;
            }

            reporter.BeginTest(test);
            if (!ok)
            {
                reporter.EndTest(test, 0, TestCaseState.Failed);
                return;
            }
            if (test.HasIgnoreAttribute)
            {
                reporter.Ignore(test.IgnoreMessage);
                reporter.EndTest(test, 0, TestCaseState.Ignored);
                return;
            }

            bool setUpDone = false;
            TestCaseState runState = TestCaseState.Failed;
            try
            {
                fixture.SetUp();
                setUpDone = true;
                test.Run(fixture, reporter);
                runState = TestCaseState.Success;
            }
            catch (System.Exception e)
            {
                if (Exception.IsExpected(e, typeof(NUnit.Framework.IgnoreException).FullName))
                {
                    runState = TestCaseState.Ignored;
                    reporter.Ignore(Exception.GetMessage(e));
                }
                else
                {
                    Exception.Report(e, reporter);
                }
            }

            TestCaseState state = TestCaseState.Failed;
            if (setUpDone)
            {
                try
                {
                    fixture.TearDown();
                    state = runState;
                }
                catch (System.Exception e)
                {
                    Exception.Report(e, reporter);
                }
            }
            reporter.EndTest(test, 0, state);
        }

        private void Run(bool ok, TestFixture fixture, Test test, TestCase testCase, TestReporter reporter)
        {
            reporter.BeginTest(testCase);
            if (!ok)
            {
                reporter.EndTest(testCase, 0, TestCaseState.Failed);
                return;
            }

            bool setUpDone = false;
            TestCaseState runState = TestCaseState.Failed;
            try
            {
                fixture.SetUp();
                setUpDone = true;
                testCase.Run(fixture, reporter);
                runState = TestCaseState.Success;
            }
            catch (System.Exception e)
            {
                Exception.Report(e, reporter);
            }

            TestCaseState state = TestCaseState.Failed;
            if (setUpDone)
            {
                try
                {
                    fixture.TearDown();
                    state = runState;
                }
                catch (System.Exception e)
                {
                    Exception.Report(e, reporter);
                }
            }
            reporter.EndTest(testCase, 0, state);
        }
    }
}
