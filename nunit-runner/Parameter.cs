//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    class Parameter
    {
        System.Reflection.ParameterInfo paramInfo;
        object dataAttribute;
        System.Reflection.MethodInfo methodInfo;

        public Parameter(System.Reflection.ParameterInfo paramInfo)
        {
            this.paramInfo = paramInfo;
            dataAttribute = Reflection.GetAttribute(paramInfo, typeof(NUnit.Framework.ParameterDataAttribute), false);
            if (dataAttribute == null)
                throw new System.Exception("Missing ParameterData Attribute");
            methodInfo = dataAttribute.GetType().GetMethod("GetData");
        }

        public System.Collections.IEnumerable GetData()
        {
            return (System.Collections.IEnumerable)methodInfo.Invoke(dataAttribute, new object[] { paramInfo });
        }
    }
}
