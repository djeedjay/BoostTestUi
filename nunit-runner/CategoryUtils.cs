//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

namespace TestRunner
{
    static class CategoryUtils
    {
        public static System.Collections.Generic.List<string> GetCategories(System.Reflection.ICustomAttributeProvider provider)
        {
            var categories = new System.Collections.Generic.List<string>();
            foreach (var category in Reflection.GetAttributes(provider, typeof(NUnit.Framework.CategoryAttribute), false))
            {
                var categoryName = Reflection.GetProperty(category, "Name");
                if (categoryName != null)
                    categories.Add(categoryName as string);
            }
            return categories;
        }
    }
}
