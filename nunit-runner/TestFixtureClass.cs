//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    class TestFixtureClass : TestItem
    {
        string fullName;
        public System.Collections.Generic.List<Test> Tests { get; private set; }
        public System.Collections.Generic.List<string> Categories { get; private set; }
        System.Reflection.ConstructorInfo ctorInfo;
        System.Collections.Generic.List<System.Reflection.MethodInfo> testFixtureSetUp = new System.Collections.Generic.List<System.Reflection.MethodInfo>();
        System.Collections.Generic.List<System.Reflection.MethodInfo> setUp = new System.Collections.Generic.List<System.Reflection.MethodInfo>();
        System.Collections.Generic.List<System.Reflection.MethodInfo> tearDown = new System.Collections.Generic.List<System.Reflection.MethodInfo>();
        System.Collections.Generic.List<System.Reflection.MethodInfo> testFixtureTearDown = new System.Collections.Generic.List<System.Reflection.MethodInfo>();
        TestFixtureClass baseClass;
        object[] parameters;

        public bool HasExplicitAttribute { get; private set; }

        public bool HasIgnoreAttribute { get; private set; }

        public string IgnoreMessage { get; private set; }

        public TestFixture CreateInstance()
        {
            return new TestFixture(this, ctorInfo.Invoke(parameters));
        }

        private static int CompareMethodInfoByName(System.Reflection.MethodInfo x, System.Reflection.MethodInfo y)
        {
            return x.Name.CompareTo(y.Name);
        }

        public TestFixtureClass(string name, System.Type type, object[] parameters) : base(name)
        {
            if (type.BaseType == null || type.BaseType.GetConstructor(new System.Type[0]) == null)
                baseClass = null;
            else
                baseClass = new TestFixtureClass(name, type.BaseType, null);
            this.parameters = parameters;

            fullName = type.FullName;
            HasExplicitAttribute = Reflection.HasAttribute(type, typeof(NUnit.Framework.ExplicitAttribute), false);
            var ignoreAttribute = Reflection.GetAttribute(type, typeof(NUnit.Framework.IgnoreAttribute), false);
            HasIgnoreAttribute = ignoreAttribute != null;
            if (HasIgnoreAttribute)
                IgnoreMessage = Reflection.GetProperty(ignoreAttribute, "Reason") as string;
            Tests = new System.Collections.Generic.List<Test>();
            Categories = CategoryUtils.GetCategories(type);

            ctorInfo = type.GetConstructor(MethodHelper.GetArgumentTypes(parameters));
            if (ctorInfo == null)
                throw new System.Exception("No default constructor");

            var methods = type.GetMethods();
            System.Array.Sort(methods, CompareMethodInfoByName);
            foreach (var methodInfo in methods)
            {
                var hasTestAttribute = Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TestAttribute), false);
                var hasTestCaseAttribute = Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TestCaseAttribute), false);
                var hasTestCaseSourceAttribute = Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TestCaseSourceAttribute), false);
                var hasTestFixtureSetUpAttribute = Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TestFixtureSetUpAttribute), false);
                var hasSetUpAttribute = Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.SetUpAttribute), false);
                var hasTearDownAttribute = Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TearDownAttribute), false);
                var hasTestFixtureTearDownAttribute = Reflection.HasAttribute(methodInfo, typeof(NUnit.Framework.TestFixtureTearDownAttribute), false);
                var hasNUnitAttribute = hasTestAttribute || hasTestFixtureSetUpAttribute || hasSetUpAttribute || hasTearDownAttribute || hasTestFixtureTearDownAttribute;

                if (hasTestAttribute ||
                    hasTestCaseAttribute ||
                    hasTestCaseSourceAttribute ||
                    !hasNUnitAttribute && methodInfo.Name.Substring(0, 4).ToLower() == "test" && methodInfo.GetParameters().Length == 0)
                    Tests.Add(new Test(methodInfo));

                if (hasSetUpAttribute)
                    setUp.Add(methodInfo);

                if (hasTearDownAttribute)
                    tearDown.Add(methodInfo);

                if (hasTestFixtureTearDownAttribute)
                    testFixtureTearDown.Add(methodInfo);
            }
        }

        public void TestFixtureSetUp(object fixture)
        {
            if (baseClass != null)
                baseClass.TestFixtureSetUp(fixture);
            foreach (var method in testFixtureSetUp)
                method.Invoke(fixture, null);
        }

        public void SetUp(object fixture)
        {
            if (baseClass != null)
                baseClass.SetUp(fixture);
            foreach (var method in setUp)
                method.Invoke(fixture, null);
        }

        public void TearDown(object fixture)
        {
            foreach (var method in tearDown)
                method.Invoke(fixture, null);
            if (baseClass != null)
                baseClass.TearDown(fixture);
        }

        public void TestFixtureTearDown(object fixture)
        {
            foreach (var method in testFixtureTearDown)
                method.Invoke(fixture, null);
            if (baseClass != null)
                baseClass.TestFixtureTearDown(fixture);
        }
    }
}
