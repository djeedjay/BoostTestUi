//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    class TestNamespace : TestItem
    {
        public System.Collections.Generic.List<TestItem> Items { get; private set; }

        public TestNamespace(string name) : base(name)
        {
            Items = new System.Collections.Generic.List<TestItem>();
        }

        public void Add(string path, System.Type type, object[] arguments)
        {
            int index = path.IndexOf('.');
            if (index < 0)
            {
                Items.Add(new TestFixtureClass(path, type, arguments));
                return;
            }

            string name = path.Substring(0, index);
            string tail = path.Substring(index + 1);
            TestNamespace ns = Items.Find(item => item.Name == name) as TestNamespace;
            if (ns != null)
            {
                ns.Add(tail, type, arguments);
                return;
            }

            TestNamespace newNs = new TestNamespace(name);
            newNs.Add(tail, type, arguments);
            Items.Add(newNs);
        }
    }
}
