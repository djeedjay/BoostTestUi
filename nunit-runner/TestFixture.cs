//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
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
}
