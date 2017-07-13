//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    using System.Linq;
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

        public TestCase(System.Reflection.MethodInfo methodInfo, object[] args) : base(MethodHelper.GetDisplayName(methodInfo, args))
        {
            this.methodInfo = methodInfo.IsGenericMethod ?
                methodInfo.MakeGenericMethod(args.Select(arg => arg.GetType()).ToArray()) :
                methodInfo;
            this.arguments = args;
        }

        public void Run(TestFixture fixture, TestReporter reporter)
        {
            try
            {
                fixture.Invoke(methodInfo, arguments);
            }
            catch (System.Exception e)
            {
                if (Exception.IsSuccessException(e))
                    reporter.Pass((string)Reflection.GetProperty(Exception.InnerException(e), "Message"));
                else
                    throw;
            }
        }
    }
}
