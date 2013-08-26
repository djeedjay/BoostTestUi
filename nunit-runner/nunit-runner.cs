//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.nunit.org/ for the NUnit test library home page.

namespace TestRunner
{
	using System.Linq;

	class TestReporter
	{
		private static int BoolAsInt(bool b)
		{
			return b ? 1 : 0;
		}

		public void BeginRun()
		{
			System.Console.WriteLine("#RunStarted {0}", 0);
		}

		public void BeginSuite(TestItem suite)
		{
			System.Console.WriteLine("#SuiteStarted {0}", suite.Id);
			System.Console.WriteLine("Entering test suite {0}", suite.Name);
		}

		public void BeginTest(TestItem test)
		{
			System.Console.WriteLine("#TestStarted {0}", test.Id);
			System.Console.WriteLine("Entering test case {0}", test.Name);
		}

		public void Exception(string message)
		{
			System.Console.WriteLine("Exception: {0}", message);
			System.Console.WriteLine("#Exception");
		}

		public void EndTest(TestItem test, int ms, bool success)
		{
			System.Console.WriteLine("Leaving test case {0}", test.Name);
			System.Console.WriteLine("#TestFinished {0} {1} {2}", test.Id, ms, BoolAsInt(success));
		}

		public void EndSuite(TestItem suite, int ms)
		{
			System.Console.WriteLine("Leaving test suite {0}", suite.Name);
			System.Console.WriteLine("#SuiteFinished {0} {1}", suite.Id, ms);
		}

		public void EndRun()
		{
			System.Console.WriteLine("#RunFinished {0}", 0);
		}
	};

	class TestItem
	{
		public int Id { get; private set; }

		public string Name { get; private set; }

		static int nextId = 0;

		public TestItem(string name)
		{
			Id = ++nextId;
			Name = name;
		}
	}

	static class Reflection
	{
		public static bool IsA(System.Type type, string typeName)
		{
			if (type.FullName == typeName)
				return true;
			else if (type == typeof(object))
				return false;
			else
				return IsA(type.BaseType, typeName);
		}

		public static bool IsA(System.Type testType, System.Type baseType)
		{
			return IsA(testType, baseType.FullName);
		}

		public static object GetAttribute(System.Reflection.ICustomAttributeProvider provider, string attributeName, bool inherit)
		{
			foreach (var attribute in provider.GetCustomAttributes(inherit))
			{
				if (IsA(attribute.GetType(), attributeName))
					return attribute;
			}
			return null;
		}

		public static object GetAttribute(System.Reflection.ICustomAttributeProvider provider, System.Type attributeType, bool inherit)
		{
			return GetAttribute(provider, attributeType.FullName, inherit);
		}

		public static bool HasAttribute(System.Reflection.ICustomAttributeProvider provider, string attributeName, bool inherit)
		{
			foreach (var attribute in provider.GetCustomAttributes(inherit))
			{
				if (IsA(attribute.GetType(), attributeName))
					return true;
			}
			return false;
		}

		public static bool HasAttribute(System.Reflection.ICustomAttributeProvider provider, System.Type attributeType, bool inherit)
		{
			return HasAttribute(provider, attributeType.FullName, inherit);
		}
	}

	static class Exception
	{
		private static string GetClientStackTrace(System.Exception e)
		{
			int index = e.StackTrace.LastIndexOf("NUnit.Framework.Assert.");
			if (index >= 0)
				index = e.StackTrace.IndexOf("\n", index);
			return index < 0 ? e.StackTrace : e.StackTrace.Substring(index + 1);
		}

		public static bool IsExpected(System.Reflection.TargetInvocationException e)
		{
			return IsExpected(e.InnerException);
		}

		public static bool IsExpected(System.Reflection.TargetInvocationException e, string expectedExceptionName)
		{
			return IsExpected(e.InnerException, expectedExceptionName);
		}

		public static bool IsExpected(System.Exception e)
		{
			if (e is System.Reflection.TargetInvocationException)
			{
				return IsExpected(e as System.Reflection.TargetInvocationException);
			}
			return e.GetType().FullName == typeof(NUnit.Framework.SuccessException).FullName;
		}

		public static bool IsExpected(System.Exception e, string expectedExceptionName)
		{
			if (e is System.Reflection.TargetInvocationException)
			{
				return IsExpected(e as System.Reflection.TargetInvocationException, expectedExceptionName);
			}
			return
				e.GetType().FullName == typeof(NUnit.Framework.SuccessException).FullName ||
				e.GetType().FullName == expectedExceptionName;
		}

		public static void Report(System.Reflection.TargetInvocationException e, TestReporter reporter)
		{
			Report(e.InnerException, reporter);
		}

		public static void Report(System.Exception e, TestReporter reporter)
		{
			if (e is System.Reflection.TargetInvocationException)
			{
				Report(e as System.Reflection.TargetInvocationException, reporter);
				return;
			}

			reporter.Exception(e.Message + "\n" + GetClientStackTrace(e));
		}
	}

	class Parameter
	{
		System.Reflection.ParameterInfo paramInfo;
		object dataAttribute;
		System.Reflection.MethodInfo methodInfo;

		public Parameter(System.Reflection.ParameterInfo paramInfo)
		{
			this.paramInfo = paramInfo;
			dataAttribute = Reflection.GetAttribute(paramInfo, typeof(NUnit.Framework.ParameterDataAttribute), false);
			if (dataAttribute == null)
				throw new System.Exception("Missing ParameterData Attribute");
			methodInfo = dataAttribute.GetType().GetMethod("GetData");
		}

		public System.Collections.IEnumerable GetData()
		{
			return (System.Collections.IEnumerable)methodInfo.Invoke(dataAttribute, new object[] { paramInfo });
		}
	}

	class TestCase : TestItem
	{
		System.Reflection.MethodInfo methodInfo;
		object[] arguments;

		static private string FormatArg(object arg)
		{
			if (arg == null)
				return "null";
			if (arg.GetType() == typeof(string))
				return "\"" + arg.ToString() + "\"";
			return arg.ToString();
		}

		static private string FormatMethod(ref System.Reflection.MethodInfo methodInfo, object[] args)
		{
			string s = methodInfo.Name;
			if (methodInfo.IsGenericMethod)
			{
				s += "<";
				bool first = true;
				System.Type[] argTypes = args.Select(arg => arg.GetType()).ToArray();
				methodInfo = methodInfo.MakeGenericMethod(argTypes);
				foreach (var type in methodInfo.GetGenericArguments())
				{
					if (first)
						first = false;
					else
						s += ",";
					s += type.Name;
				}
				s += ">";
			}

			s += "(";
			for (int i = 0; i < args.GetLength(0); ++i)
			{
				if (i > 0)
					s += ",";
				s += FormatArg(args[i]);
			}
			s += ")";
			return s;
		}

		public TestCase(System.Reflection.MethodInfo methodInfo, object[] args) : base(FormatMethod(ref methodInfo, args))
		{
			this.methodInfo = methodInfo;
			this.arguments = args;
		}

		public void Run(TestFixture fixture, TestReporter reporter)
		{
			reporter.BeginTest(this);
			bool success = false;
			bool setUpDone = false;
			try
			{
				fixture.SetUp();
				setUpDone = true;
				fixture.Invoke(methodInfo, arguments);
				success = true;
			}
			catch (System.Exception e)
			{
				if (Exception.IsExpected(e))
					success = true;
				else
					Exception.Report(e, reporter);
			}
			finally
			{
				if (setUpDone)
					fixture.TearDown();
			}
			reporter.EndTest(this, 0, success);
		}
	}

	class Test : TestItem
	{
		System.Reflection.MethodInfo methodInfo;

		public bool HasExplicitAttribute { get; private set; }

		public System.Collections.Generic.List<TestCase> TestCases { get; private set; }

		public Test(System.Reflection.MethodInfo methodInfo) : base(methodInfo.Name)
		{
			this.methodInfo = methodInfo;
			this.TestCases = new System.Collections.Generic.List<TestCase>();
			HasExplicitAttribute = Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.ExplicitAttribute), false);

			var parameterData = new System.Collections.Generic.List<System.Collections.IEnumerable>();
			foreach (var paramInfo in methodInfo.GetParameters())
			{
				var p = new Parameter(paramInfo);
				parameterData.Add(p.GetData());
			}
			if (parameterData.Count == 0)
				return;

			if (Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.SequentialAttribute), false))
				AddSequentialParameterSet(parameterData);
			else
				AddCombinatorialParamaterSet(parameterData, 0, new object[parameterData.Count]);
		}

		void AddSequentialParameterSet(System.Collections.Generic.List<System.Collections.IEnumerable> parameterData)
		{
			int args = 0;
			foreach (var values in parameterData)
			{
				int count = 0;
				foreach (var value in values)
					++count;
				args = System.Math.Max(args, count);
			}

			var parameterSets = new System.Collections.Generic.List<object[]>();
			for (int i = 0; i < args; ++i)
				parameterSets.Add(new object[parameterData.Count]);

			int arg = 0;
			foreach (var values in parameterData)
			{
				int i = 0;
				foreach (var value in values)
				{
					parameterSets[i][arg] = value;
					++i;
				}
				++arg;
			}

			foreach (var parameterSet in parameterSets)
			{
				TestCases.Add(new TestCase(methodInfo, parameterSet));
			}
		}

		void AddCombinatorialParamaterSet(System.Collections.Generic.List<System.Collections.IEnumerable> parameterData, int parameterIndex, object[] parameterSet)
		{
			if (parameterIndex == parameterData.Count)
			{
				TestCases.Add(new TestCase(methodInfo, (object[])parameterSet.Clone()));
			}
			else
			{
				foreach (var value in parameterData[parameterIndex])
				{
					parameterSet[parameterIndex] = value;
					AddCombinatorialParamaterSet(parameterData, parameterIndex + 1, parameterSet);
				}
			}
		}

		public void Run(TestFixture fixture, TestReporter reporter)
		{
			var exception = Reflection.GetAttribute(methodInfo, typeof(NUnit.Framework.ExpectedExceptionAttribute), false);
			string expectedExceptionName = null;
			if (exception != null)
				expectedExceptionName = (string)exception.GetType().GetProperty("ExpectedExceptionName").GetValue(exception, null);

			if (TestCases.Count > 0)
			{
				reporter.BeginSuite(this);
				foreach (var testCase in TestCases)
				{
					testCase.Run(fixture, reporter);
				}
				reporter.EndSuite(this, 0);
				return;
			}

			reporter.BeginTest(this);
			bool success = false;
			bool setUpDone = false;
			try
			{
				fixture.SetUp();
				setUpDone = true;
				fixture.Invoke(methodInfo);
				success = true;
			}
			catch (System.Exception e)
			{
				if (Exception.IsExpected(e, expectedExceptionName))
					success = true;
				else
					Exception.Report(e, reporter);
			}
			finally
			{
				if (setUpDone)
					fixture.TearDown();
			}
			reporter.EndTest(this, 0, success);
		}
	}

	class TestFixture
	{
		TestFixtureClass fixtureClass;
		object instance;

		public TestFixture(TestFixtureClass fixtureClass, object instance)
		{
			this.fixtureClass = fixtureClass;
			this.instance = instance;
		}

		public void TestFixtureSetUp()
		{
			fixtureClass.TestFixtureSetUp(instance);
		}

		public void SetUp()
		{
			fixtureClass.SetUp(instance);
		}

		public void Invoke(System.Reflection.MethodInfo method)
		{
			method.Invoke(instance, null);
		}

		public void Invoke(System.Reflection.MethodInfo method, object[] parameters)
		{
			method.Invoke(instance, parameters);
		}

		public void TearDown()
		{
			fixtureClass.TearDown(instance);
		}

		public void TestFixtureTearDown()
		{
			fixtureClass.TestFixtureTearDown(instance);
		}
	}

	class TestFixtureClass : TestItem
	{
		string fullName;
		public System.Collections.Generic.List<Test> Tests { get; private set; }
		System.Reflection.ConstructorInfo ctorInfo;
		System.Collections.Generic.List<System.Reflection.MethodInfo> testFixtureSetUp = new System.Collections.Generic.List<System.Reflection.MethodInfo>();
		System.Collections.Generic.List<System.Reflection.MethodInfo> setUp = new System.Collections.Generic.List<System.Reflection.MethodInfo>();
		System.Collections.Generic.List<System.Reflection.MethodInfo> tearDown = new System.Collections.Generic.List<System.Reflection.MethodInfo>();
		System.Collections.Generic.List<System.Reflection.MethodInfo> testFixtureTearDown = new System.Collections.Generic.List<System.Reflection.MethodInfo>();

		public bool HasExplicitAttribute { get; private set; }

		public TestFixture CreateInstance()
		{
			return new TestFixture(this, ctorInfo.Invoke(null));
		}

		public TestFixtureClass(string name, System.Type type) : base(name)
		{
			fullName = type.FullName;
			HasExplicitAttribute = Reflection.HasAttribute(type, typeof(NUnit.Framework.ExplicitAttribute), false);
			Tests = new System.Collections.Generic.List<Test>();
			ctorInfo = type.GetConstructor(System.Type.EmptyTypes);
			if (ctorInfo == null)
				throw new System.Exception("No default constructor");

			var methods = type.GetMethods();
			foreach (var methodInfo in methods)
			{
				if (Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TestAttribute), false) &&
					!Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.IgnoreAttribute), false))
					Tests.Add(new Test(methodInfo));

				if (Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TestFixtureSetUpAttribute), false))
					testFixtureSetUp.Add(methodInfo);

				if (Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.SetUpAttribute), false))
					setUp.Add(methodInfo);

				if (Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TearDownAttribute), false))
					tearDown.Add(methodInfo);

				if (Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TestFixtureTearDownAttribute), false))
					testFixtureTearDown.Add(methodInfo);
			}
		}

		public void TestFixtureSetUp(object fixture)
		{
			foreach (var method in testFixtureSetUp)
				method.Invoke(fixture, null);
		}

		public void SetUp(object fixture)
		{
			foreach (var method in setUp)
				method.Invoke(fixture, null);
		}

		public void TearDown(object fixture)
		{
			foreach (var method in tearDown)
				method.Invoke(fixture, null);
		}

		public void TestFixtureTearDown(object fixture)
		{
			foreach (var method in testFixtureTearDown)
				method.Invoke(fixture, null);
		}
	}

	class TestNamespace : TestItem
	{
		public System.Collections.Generic.List<TestItem> Items { get; private set; }
		
		public TestNamespace(string name) : base(name)
		{
			Items = new System.Collections.Generic.List<TestItem>();
		}

		public void Add(string path, System.Type type)
		{
			int index = path.IndexOf('.');
			if (index < 0)
			{
				Items.Add(new TestFixtureClass(path, type));
				return;
			}

			string name = path.Substring(0, index);
			string tail = path.Substring(index + 1);
			TestNamespace ns = Items.Find(item => item.Name == name) as TestNamespace;
			if (ns != null)
			{
				ns.Add(tail, type);
				return;
			}

			TestNamespace newNs = new TestNamespace(name);
			newNs.Add(tail, type);
			Items.Add(newNs);
		}
	}

	class TestLib
	{
		TestNamespace global;
		System.Random random = null;

		public TestLib(string name, int randomSeed)
		{
			global = new TestNamespace("Global");
			random = (randomSeed < 0) ? null : new System.Random(randomSeed);
			if (random != null)
				System.Console.WriteLine("Test cases order is shuffled using seed: {0}", randomSeed);

			var lib = System.Reflection.Assembly.LoadFile(name);
			System.Type[] classes = lib.GetTypes();
			foreach (System.Type type in classes)
			{
				if (type.IsClass && Reflection.HasAttribute(type, typeof(NUnit.Framework.TestFixtureAttribute), false))
					global.Add(type.FullName, type);
			}
		}

		private static string Label(Test test, TestCase testCase)
		{
			char type = test.HasExplicitAttribute ? 'c' : 'C';
			return string.Format("{0}{1}:{2}", type, testCase.Id, testCase.Name);
		}

		private static string Label(Test test)
		{
			string type = test.TestCases.Count == 0 ? "C" : "S";
			if (test.HasExplicitAttribute)
				type = type.ToLower();
			return string.Format("{0}{1}:{2}", type, test.Id, test.Name);
		}

		private static string Label(TestFixtureClass fixture)
		{
			char type = fixture.HasExplicitAttribute ? 's' : 'S';
			return string.Format("{0}{1}:{2}", type, fixture.Id, fixture.Name);
		}

		private static string Label(TestNamespace ns)
		{
			return string.Format("S{0}:{1}", ns.Id, ns.Name);
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

			bool setupDone = false;
			TestFixture fixture = null;
			try
			{
				fixture = fixtureClass.CreateInstance();

				fixture.TestFixtureSetUp();
				setupDone = true;
				foreach (var test in GetItemOrder(fixtureClass.Tests))
				{
					if (filter.Match(path + '.' + test.Name))
						Run(path, fixture, test, filter, reporter);
				}
			}
			catch (System.Exception e)
			{
				Exception.Report(e, reporter);
			}
			finally
			{
				if (setupDone)
					TestFixtureTearDown(fixture, reporter);
			}
			reporter.EndSuite(fixtureClass, 0);
		}

		static private void TearDown(TestFixture fixture, TestReporter reporter)
		{
			try
			{
				fixture.TearDown();
			}
			catch (System.Exception e)
			{
				Exception.Report(e, reporter);
			}
		}

		private void Run(string path, TestFixture fixture, Test test, TestCaseFilter filter, TestReporter reporter)
		{
			try
			{
				if (test.TestCases.Count == 0)
				{
					test.Run(fixture, reporter);
				}
				else
				{
					reporter.BeginSuite(test);
					foreach (var testCase in GetItemOrder(test.TestCases))
					{
						if (filter.Match(path + '.' + test.Name + '.' + testCase.Name))
							testCase.Run(fixture, reporter);
					}
					reporter.EndSuite(test, 0);
				}
			}
			catch (System.Exception e)
			{
				Exception.Report(e, reporter);
			}
		}
	}

	interface TestCaseFilter
	{
		bool Match(string name);
	}

	class TestCaseDefaultFilter : TestCaseFilter
	{
		public bool Match(string name)
		{
			return true;
		}
	}

	class TestCaseNameFilter : TestCaseFilter
	{
		System.Collections.Generic.List<string> names = new System.Collections.Generic.List<string>();

		public System.Collections.Generic.IList<string> Names { get { return names; } }

		public TestCaseNameFilter(string arg)
		{
			int parens = 0;
			int angles = 0;
			bool inQuotes = false;
			int begin = 0;
			for (int i = 0; i < arg.Length; ++i)
			{
				switch (arg[i])
				{
				case '(':
					if (!inQuotes)
						++parens;
					break;
				case ')':
					if (!inQuotes)
						--parens;
					break;
				case '<':
					if (!inQuotes)
						++angles;
					break;
				case '>':
					if (!inQuotes)
						--angles;
					break;
				case '"':
					inQuotes = !inQuotes;
					break;
				case ',':
					if (!inQuotes && angles == 0 && parens == 0)
					{
						names.Add(arg.Substring(begin, i - begin));
						begin = i + 1;
					}
					break;
				}
			}
			if (begin < arg.Length)
				names.Add(arg.Substring(begin));
		}

		public bool Match(string name)
		{
			int depth = Depth(name);
			string n = name + ".";
			return names.Count == 0 || names.Exists(item => n.StartsWith(Prefix(item, depth)));
		}

		static public string Prefix(string path, int depth)
		{
			if (depth == 0)
				return "";

			int index = -1;
			for (int i = 0; i < depth; ++i)
			{
				index = path.IndexOf('.', index + 1);
				if (index < 0)
					break;
			}
			return index < 0 ? path + "." : path.Substring(0, index + 1);
		}

		static public int Depth(string path)
		{
			if (path == "")
				return 0;

			int depth = 0;
			int index = 0;
			for (;;)
			{
				index = path.IndexOf('.', index + 1);
				if (index < 0)
					break;
				++depth;
			}
			return depth + 1;
		}
	}

	class Program
	{
		static bool list = false;
		static bool wait = false;
		static bool run = false;
		static int randomize = -1;
		static TestCaseFilter filter = null;

		static void HandleOption(string arg)
		{
			int i = arg.IndexOfAny(":=".ToCharArray());
			string value = null;
			if (i > 0)
			{
				value = arg.Substring(i + 1);
				arg = arg.Substring(0, i);
			}

			if (arg.ToLower() == "list")
			{
				list = true;
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
			var libs = ReadOptions(args);
			foreach (string lib in libs)
			{
				var filename = System.IO.Path.GetFullPath(lib);
				using (AssemblyResolver resolver = new AssemblyResolver(System.IO.Path.GetDirectoryName(filename)))
				{
					TestLib testLib = new TestLib(filename, randomize);
					if (list)
						testLib.List();
					if (wait)
						Wait();
					if (run)
						testLib.Run(filter, new TestReporter());
				}
			}
		}
	}
}
