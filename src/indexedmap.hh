/*
 *  wadmake: a WAD manipulation utility.
 *  Copyright (C) 2015  Alex Mayfield
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// IndexedMap implements map-like and vector-like behavior in one container.
//
// Pushing into the container, you get a mapping that looks a lot like a vector
// at first blush - indexes are mapped to elements in a sequence.  Unlike a
// vector, however, removing elements from the middle does not modify the
// indexes of the elements that follow the removed element...unless you
// explicitly call reindex, at which point the entire container is reindexed
// from 0.
//
// For my own uses, I need to know what the index of an element is just by
// looking at it, and C++11 doesn't have a built-in bimap, so all types that
// you use in this container must have a publicly-accessable member called
// id of type size_t.

#ifndef INDEXEDMAP_HH
#define INDEXEDMAP_HH

#include <limits>
#include <list>
#include <unordered_map>

namespace WADmake {

template <class T>
class IndexedMap {
protected:
	std::list<std::shared_ptr<T>> elements;
	size_t nextid;
	std::unordered_map<size_t, std::weak_ptr<T>> elementids;
public:
	typedef typename std::unordered_map<size_t, std::weak_ptr<T>>::iterator iterator;
	IndexedMap();
	T& operator[](size_t pos);
	T& at(size_t pos);
	iterator begin();
	iterator end();
	iterator find(size_t index);
	void push_back(T&& element);
	void reindex();
};

template <class T>
IndexedMap<T>::IndexedMap() : nextid(0) { }

template <class T>
T& IndexedMap<T>::operator[](size_t pos) {
	if (!this->elementids[pos].expired()) {
		auto ptr = this->elementids[pos].lock();
		if (ptr) {
			return *ptr;
		} else {
			throw std::runtime_error("Couldn't lock element");
		}
	} else {
		auto ptr = std::make_shared<T>();
		ptr->id = pos;
		this->elementids.emplace(std::make_pair(pos, ptr));
		this->elements.push_back(ptr);
		return *ptr;
	}
}

template <class T>
T& IndexedMap<T>::at(size_t pos) {
	auto ptr = this->elementids.at(pos).lock();
	if (ptr) {
		return *ptr;
	} else {
		throw std::runtime_error("Couldn't lock element");
	}
}

template <class T>
typename IndexedMap<T>::iterator IndexedMap<T>::begin() {
	return this->elementids.begin();
}

template <class T>
typename IndexedMap<T>::iterator IndexedMap<T>::end() {
	return this->elementids.end();
}

template <class T>
typename IndexedMap<T>::iterator IndexedMap<T>::find(size_t index) {
	return this->elementids.find(index);
}

template <class T>
void IndexedMap<T>::push_back(T&& element) {
	if (this->nextid == std::numeric_limits<size_t>::max()) {
		throw std::runtime_error("Too many Element IDs");
	}

	element.id = this->nextid;
	auto eleptr = std::make_shared<T>(std::move(element));
	this->elementids.emplace(std::make_pair(this->nextid, eleptr));
	this->elements.push_back(std::move(eleptr));
	this->nextid += 1;
}

template <class T>
void IndexedMap<T>::reindex() {
	this->elementids.clear();
	this->nextid = 1;
	for (auto eleptr : this->elements) {
		eleptr->id = this->nextid;
		this->elementids.emplace(std::make_pair(this->nextid, eleptr));
		this->nextid += 1;
	}
}

}

#endif
