//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
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
}
