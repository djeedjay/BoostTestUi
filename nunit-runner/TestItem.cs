//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
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
}
