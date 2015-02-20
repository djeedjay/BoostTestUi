//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    public class MethodHelper
    {
		public static System.Type[] GetArgumentTypes(object[] parameters)
		{
			if (parameters == null)
				return System.Type.EmptyTypes;

			System.Type[] types = new System.Type[parameters.Length];
			for (int i = 0; i < parameters.Length; ++i)
				types[i] = parameters[i].GetType();
			return types;
		}

		public static string GetDisplayName(System.Reflection.ConstructorInfo ctor, object[] arglist)
		{
			return GetDisplayName(ctor, ctor.DeclaringType.Name, arglist);
		}

		public static string GetDisplayName(System.Reflection.MethodInfo method, object[] arglist)
		{
			return GetDisplayName(method, method.Name, arglist);
		}

        private static string GetDisplayName(System.Reflection.MethodBase method, string name, object[] arglist)
        {
            System.Text.StringBuilder sb = new System.Text.StringBuilder(name);

            if (method.IsGenericMethod)
            {
                sb.Append("<");
                int cnt = 0;
                foreach (var t in method.GetGenericArguments())
                {
                    if (cnt++ > 0)
						sb.Append(",");
                    sb.Append(t.Name);
                }
                sb.Append(">");
            }

            if (arglist != null)
            {
                sb.Append("(");

                for (int i = 0; i < arglist.Length; ++i)
                {
                    if (i > 0) 
						sb.Append(",");
                    sb.Append(GetDisplayString(arglist[i]));
                }

                sb.Append(")");
            }

            return sb.ToString();
        }

        private static string GetDisplayString(object arg)
        {
            string display = arg == null 
                ? "null" 
                : System.Convert.ToString(arg, System.Globalization.CultureInfo.InvariantCulture);

            if (arg is double)
            {
                double d = (double)arg;

                if (double.IsNaN(d))
                    display = "double.NaN";
                else if (double.IsPositiveInfinity(d))
                    display = "double.PositiveInfinity";
                else if (double.IsNegativeInfinity(d))
                    display = "double.NegativeInfinity";
                else if (d == double.MaxValue)
                    display = "double.MaxValue";
                else if (d == double.MinValue)
                    display = "double.MinValue";
                else
                {
                    if (display.IndexOf('.') == -1)
                        display += ".0";
                    display += "d";
                }
            }
            else if (arg is float)
            {
                float f = (float)arg;

                if (float.IsNaN(f))
                    display = "float.NaN";
                else if (float.IsPositiveInfinity(f))
                    display = "float.PositiveInfinity";
                else if (float.IsNegativeInfinity(f))
                    display = "float.NegativeInfinity";
                else if (f == float.MaxValue)
                    display = "float.MaxValue";
                else if (f == float.MinValue)
                    display = "float.MinValue";
                else
                {
                    if (display.IndexOf('.') == -1)
                        display += ".0";
                    display += "f";
                }
            }
            else if (arg is decimal)
            {
                decimal d = (decimal)arg;
                if (d == decimal.MinValue)
                    display = "decimal.MinValue";
                else if (d == decimal.MaxValue)
                    display = "decimal.MaxValue";
                else
                    display += "m";
            }
            else if (arg is long)
            {
                long l = (long)arg;
                if (l == long.MinValue)
                    display = "long.MinValue";
                else if (l == long.MinValue)
                    display = "long.MaxValue";
                else
                    display += "L";
            }
            else if (arg is ulong)
            {
                ulong ul = (ulong)arg;
                if (ul == ulong.MinValue)
                    display = "ulong.MinValue";
                else if (ul == ulong.MinValue)
                    display = "ulong.MaxValue";
                else
                    display += "UL";
            }
            else if (arg is string)
            {
                System.Text.StringBuilder sb = new System.Text.StringBuilder();
                sb.Append("\"");
                foreach (char c in (string)arg)
                    sb.Append(EscapeControlChar(c));
                sb.Append("\"");
                display = sb.ToString();
            }
            else if (arg is char)
            {
                display = "\'" + EscapeControlChar((char)arg) + "\'";
            }
            else if (arg is int)
            {
                int ival = (int)arg;
                if (ival == int.MaxValue)
                    display = "int.MaxValue";
                else if (ival == int.MinValue)
                    display = "int.MinValue";
            }

            return display;
        }

        private static string EscapeControlChar(char c)
        {
            switch (c)
            {
            case '\'': return "\\\'";
            case '\"': return "\\\"";
            case '\\': return "\\\\";
            case '\0': return "\\0";
            case '\a': return "\\a";
            case '\b': return "\\b";
            case '\f': return "\\f";
            case '\n': return "\\n";
            case '\r': return "\\r";
            case '\t': return "\\t";
            case '\v': return "\\v";
			case '\x0085':
            case '\x2028':
            case '\x2029':
                return string.Format("\\x{0:X4}", (int)c);

            default:
                return char.IsControl(c) || (int)c > 128 ? string.Format("\\x{0:X4}", (int)c) : c.ToString();
            }
        }
    }
}
