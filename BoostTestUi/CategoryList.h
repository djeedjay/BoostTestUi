//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_CATEGORYLIST_H
#define BOOST_TESTUI_CATEGORYLIST_H

#pragma once

#pragma warning(disable: 4481) // nonstandard extension used: override specifier 'override'

#include <string>
#include <map>

namespace gj {

class CategoryList
{
public:
	typedef std::map<std::string, bool> CategoryMap;
	typedef CategoryMap::const_iterator iterator;

	CategoryList();

	void Clear();
	void Add(const std::string& category, bool select = true);
	void SetSelection(const std::string& category, bool select = true);
	bool IsDefaultSelected() const;
	bool IsSelected(const std::string& category) const;
	bool IsEmpty() const;
	iterator begin() const;
	iterator end() const;

private:
	static const std::string DefaultCategory;
	CategoryMap m_categories;
};

} // namespace gj

#endif // BOOST_TESTUI_CATEGORYLIST_H
