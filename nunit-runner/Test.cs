//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    using System.Linq;
    class Test : TestItem
    {
        System.Reflection.MethodInfo methodInfo;

        public bool HasExplicitAttribute { get; private set; }

        public bool HasIgnoreAttribute { get; private set; }

        public string IgnoreMessage { get; private set; }

        public System.Collections.Generic.List<string> Categories { get; private set; }

        public System.Collections.Generic.List<TestCase> TestCases { get; private set; }

        public Test(System.Reflection.MethodInfo methodInfo) : base(methodInfo.Name)
        {
            this.methodInfo = methodInfo;
            Categories = CategoryUtils.GetCategories(methodInfo);
            TestCases = new System.Collections.Generic.List<TestCase>();
            HasExplicitAttribute = Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.ExplicitAttribute), false);
            var ignoreAttribute = Reflection.GetAttribute(methodInfo, typeof(NUnit.Framework.IgnoreAttribute), false);
            HasIgnoreAttribute = ignoreAttribute != null;
            if (HasIgnoreAttribute)
                IgnoreMessage = Reflection.GetProperty(ignoreAttribute, "Reason") as string;

            if (Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TestCaseAttribute), false))
                AddTestCaseAttributeTests(methodInfo);
            else if (Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TestCaseSourceAttribute), false))
                AddTestCaseSourceAttributeTests(methodInfo);
            else
                AddArgumentParametersTests(methodInfo);
        }

        private void AddArgumentParametersTests(System.Reflection.MethodInfo methodInfo)
        {
            System.Collections.Generic.List<System.Collections.IEnumerable> parameterData = GetArgumentParameters(methodInfo);
            if (parameterData.Count > 0)
            {
                if (Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.SequentialAttribute), false))
                    AddSequentialParameterSet(parameterData);
                else
                    AddCombinatorialParamaterSet(parameterData, 0, new object[parameterData.Count]);
            }
        }

        private static System.Collections.Generic.List<System.Collections.IEnumerable> GetArgumentParameters(System.Reflection.MethodInfo methodInfo)
        {
            var parameterData = new System.Collections.Generic.List<System.Collections.IEnumerable>();
            foreach (var paramInfo in methodInfo.GetParameters())
            {
                var p = new Parameter(paramInfo);
                parameterData.Add(p.GetData());
            }

            return parameterData;
        }

        private void AddTestCaseSourceAttributeTests(System.Reflection.MethodInfo methodInfo)
        {
            var methodParams = methodInfo.GetParameters();
            if (methodParams.Length == 0)
                throw new System.InvalidOperationException("Test method has no parameters");

            foreach (var testCaseSourceAttr in Reflection.GetAttributes(methodInfo, typeof(NUnit.Framework.TestCaseSourceAttribute), false))
            {
                if (Reflection.GetProperty(testCaseSourceAttr, "Category") is string category)
                    Categories.Add(category);

                var sourceName = Reflection.GetProperty(testCaseSourceAttr, "SourceName") as string;
                var sourceType = Reflection.GetProperty(testCaseSourceAttr, "SourceType") as System.Type ?? methodInfo.DeclaringType;

                var bindingFlags = System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static;
                object methodArgsArray = methodArgsArray = sourceType?.GetField(sourceName, bindingFlags)?.GetValue(null);
                if (methodArgsArray == null)
                    throw new System.InvalidOperationException("Invalid sourcename and/or sourceType");

                foreach (var methodArgs in (System.Array)methodArgsArray)
                    TestCases.Add(new TestCase(methodInfo, methodArgs as object[] ?? new object[] { methodArgs as object }));
            }
        }

        private void AddTestCaseAttributeTests(System.Reflection.MethodInfo methodInfo)
        {
            var methodParams = methodInfo.GetParameters();
            if (methodParams.Length == 0)
                throw new System.InvalidOperationException("Test method has no parameters");

            foreach (var testCaseAttr in Reflection.GetAttributes(methodInfo, typeof(NUnit.Framework.TestCaseAttribute), false))
            {
                if (Reflection.GetProperty(testCaseAttr, "Category") is string category)
                    Categories.Add(category);

                if (Reflection.GetProperty(testCaseAttr, "Categories") is System.Collections.IList categories)
                    Categories.AddRange(categories.Cast<string>());

                var methodArgs = (object[])Reflection.GetProperty(testCaseAttr, "Arguments");
                TestCases.Add(new TestCase(methodInfo, methodArgs));
            }
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
            {
                expectedExceptionName = (string)Reflection.GetProperty(exception, "ExpectedExceptionName");
                if (expectedExceptionName == null)
                    expectedExceptionName = "";
            }

            int repeatCount = 1;
            var repeatAttribute = Reflection.GetAttribute(methodInfo, typeof(NUnit.Framework.RepeatAttribute), false);
            var propertiesValue = Reflection.GetProperty(repeatAttribute, "Properties");
            if (propertiesValue != null)
            {
                var properties = (System.Collections.IDictionary)propertiesValue;
                repeatCount = (int)properties["Repeat"];
            }

            try
            {
                for (int i = 0; i < repeatCount; ++i)
                    fixture.Invoke(methodInfo);
            }
            catch (System.Exception e)
            {
                if (Exception.IsSuccessException(e))
                {
                    reporter.Pass((string)Reflection.GetProperty(Exception.InnerException(e), "Message"));
                }
                else
                {
                    Exception.Handle(e, expectedExceptionName);
                    return;
                }
            }

            if (expectedExceptionName != null)
                throw new NUnit.Framework.NoExpectedException("expected " + expectedExceptionName);
        }
    }
}
