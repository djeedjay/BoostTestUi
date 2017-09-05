//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    static class Exception
    {
        private static string GetClientStackTrace(System.Exception e)
        {
            int index = e.StackTrace.LastIndexOf("NUnit.Framework.Assert.");
            if (index >= 0)
                index = e.StackTrace.IndexOf("\n", index);
            return index < 0 ? e.StackTrace : e.StackTrace.Substring(index + 1);
        }

        public static bool IsSuccessException(System.Reflection.TargetInvocationException e)
        {
            return IsSuccessException(e.InnerException);
        }

        public static bool IsExpected(System.Reflection.TargetInvocationException e, string expectedExceptionName)
        {
            return IsExpected(e.InnerException, expectedExceptionName);
        }

        public static bool IsSuccessException(System.Exception e)
        {
            if (e is System.Reflection.TargetInvocationException)
            {
                return IsSuccessException(e as System.Reflection.TargetInvocationException);
            }
            return Reflection.IsA(e.GetType(), typeof(NUnit.Framework.SuccessException).FullName);
        }

        public static System.Object InnerException(System.Reflection.TargetInvocationException e)
        {
            return InnerException(e.InnerException);
        }

        public static System.Object InnerException(System.Exception e)
        {
            if (e is System.Reflection.TargetInvocationException)
            {
                return InnerException(e as System.Reflection.TargetInvocationException);
            }
            return e;
        }

        public static bool IsExpected(System.Exception e, string expectedExceptionName)
        {
            if (e is System.Reflection.TargetInvocationException)
            {
                return IsExpected(e as System.Reflection.TargetInvocationException, expectedExceptionName);
            }
            return Reflection.IsA(e.GetType(), expectedExceptionName);
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

        public static string GetMessage(System.Reflection.TargetInvocationException e)
        {
            return GetMessage(e.InnerException);
        }

        public static string GetMessage(System.Exception e)
        {
            if (e is System.Reflection.TargetInvocationException)
                return GetMessage(e as System.Reflection.TargetInvocationException);

            return e.Message;
        }

        public static void Handle(System.Exception e, string expectedExceptionName)
        {
            if (Exception.IsSuccessException(e) || expectedExceptionName != "" && Exception.IsExpected(e, expectedExceptionName))
                return;
            throw e;
        }
    }
}
