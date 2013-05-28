/*

	Copyright 2012 Constantin Tarc <whitetrakon@gmail.com>

	This file is part of WadDirectory.

    WadDirectory is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WadDirectory is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WadDirectory.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include <boost/ptr_container/ptr_map.hpp>

namespace warstudio {
	namespace model {

// HACK: the entire code here is intransparent, hope it can be rewritten later

template <class ID, class T>	class RegistryTemplate;

template <class ID, class T>
class ProtectedRegistryTemplate
{
	friend class RegistryTemplate<ID , T>;
private:	//to be used only by RegistryTemplate. Intentionally private
	typedef typename boost::ptr_map<ID, T>		Map;


private: //to be used only by RegistryTemplate itself and not by its derived classes. Intentionally private
	bool isKnown(ID id) const {return (registry_.count(id) > 0);}
	const T& get(ID id) const {return registry_.at(id);}
    void add(ID id, T* value) {registry_.insert(id, value);}
    bool empty() const {return registry_.empty();}

	ProtectedRegistryTemplate() {}
private:
	Map	registry_;
};

template <class ID, class T>
class RegistryTemplate
{
protected:	//to be used only by friends of derived classes. Intentionally protected
    bool isKnown(ID id) const {initialize(); return registry_.isKnown(id);}
    const T& get(ID id) const {initialize(); return registry_.get(id);}

protected:	//to be used only by derived classes. Intentionally protected
	typedef typename ProtectedRegistryTemplate<ID, T>::Map		Registry;

    virtual void doInit() const = 0;	//meant to be overriden in derived classes


    void add(ID id, T* value) const	{registry_.add(id, value);}
	RegistryTemplate() {}
	virtual ~RegistryTemplate() {}

private:	//not to be used by anyone else
    void initialize() const
	{
        if (!registry_.empty())	return;
        doInit();
	}

	/*
		though we will have a lot of friends, only RegistryTemplate can call methods of the registry_
		so as long as RegistryTemplate methods ensure const-correctness it is ok to have registry_ mutable
	*/
	mutable ProtectedRegistryTemplate<ID, T>	registry_;
};

	}
}
