//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
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

        public static bool IsA<T>(System.Type testType)
        {
            return IsA(testType, typeof(T));
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

        public static System.Collections.Generic.List<object> GetAttributes(System.Reflection.ICustomAttributeProvider provider, string attributeName, bool inherit)
        {
            var attributes = new System.Collections.Generic.List<object>();
            foreach (var attribute in provider.GetCustomAttributes(inherit))
            {
                if (IsA(attribute.GetType(), attributeName))
                    attributes.Add(attribute);
            }
            return attributes;
        }

        public static System.Collections.Generic.List<object> GetAttributes(System.Reflection.ICustomAttributeProvider provider, System.Type attributeType, bool inherit)
        {
            return GetAttributes(provider, attributeType.FullName, inherit);
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

        public static object GetProperty(object obj, string propertyName)
        {
            if (obj == null)
                return null;

            var prop = obj.GetType().GetProperty(propertyName);
            if (prop == null)
                return null;

            return prop.GetValue(obj, null);
        }
    }
}
