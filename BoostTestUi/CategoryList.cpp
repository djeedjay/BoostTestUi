//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "CategoryList.h"

namespace gj {

CategoryList::CategoryList()
{
	Add(DefaultCategory, true);
}

void CategoryList::Clear()
{
	*this = CategoryList();
}

void CategoryList::Add(const std::string& category, bool select)
{
	m_categories.insert(std::make_pair(category, select));
}

void CategoryList::SetSelection(const std::string& category, bool select)
{
	m_categories[category] = select;
}

bool CategoryList::IsDefaultSelected() const
{
	return IsSelected(DefaultCategory);
}

bool CategoryList::IsSelected(const std::string& category) const
{
	auto it = m_categories.find(category);
	return it != m_categories.end() && it->second;
}

bool CategoryList::IsEmpty() const
{
	return m_categories.size() == 1;
}

CategoryList::iterator CategoryList::begin() const
{
	return m_categories.begin();
}

CategoryList::iterator CategoryList::end() const
{
	return m_categories.end();
}

const std::string CategoryList::DefaultCategory = "<Default>";

} // namespace gj
