#pragma once
#include "AllocatorOperation.h"

struct LifeCount {
	long lLifeCountRef;
	long lObserverCountRef;
	LifeCount():lLifeCountRef(0),lObserverCountRef(0) {};
	LifeCount(long lifeRef, long observerRef) :lLifeCountRef(lifeRef), lObserverCountRef(observerRef) { ; }
};

template <class T>
class StrongPtr {
	template<class T>
	friend class WeakPtr;
public:
	StrongPtr():m_is_data_dirty_(true) {;};
	StrongPtr(T * object) {
		if (object) {
			m_pObject = object;
			m_pLifeCount = new LifeCount(1, 0);
			m_is_data_dirty_ = false;
		}
		else {
			m_pObject = nullptr;
			m_pLifeCount = nullptr;
			m_is_data_dirty_ = true;
		}
	}
	StrongPtr(T * object, LifeCount * lf):m_pObject(object),m_pLifeCount(lf) {
		if (m_pObject) {
			if (m_pLifeCount) {
				m_pLifeCount->lLifeCountRef++;
				m_is_data_dirty_ = false;
			}
		}
		else {
			m_pObject = nullptr;
			m_pLifeCount = nullptr;
			m_is_data_dirty_ = true;
		}
	}

	StrongPtr(const StrongPtr & i_otherPtr) {
		if (!i_otherPtr.m_is_data_dirty_) {
			m_pObject = i_otherPtr.m_pObject;
			m_pLifeCount = i_otherPtr.m_pLifeCount;
			m_pLifeCount->lLifeCountRef++;
			m_is_data_dirty_ = false;
		}
		else {
			m_pObject = nullptr;
			m_pLifeCount = nullptr;
			m_is_data_dirty_ = true;
		}
	}
	~StrongPtr() {
		releaseReferenCount();
	}
	//disable move constructor, maybe?
	//StrongPtr(StrongPtr && i_otherPtr) = delete;
public:
	inline T * operator ->() {
		return m_pObject; 
	}
	T & operator *() { if (m_pObject != nullptr) return *(m_pObject); else return nullptr; }
	StrongPtr<T>  & operator = (StrongPtr & i_otherPtr) {
		if (&i_otherPtr == this) {
			return *this;
		}
		if (!i_otherPtr.m_is_data_dirty_) {
			releaseReferenCount();
			m_pLifeCount = i_otherPtr.m_pLifeCount;
			m_pObject = i_otherPtr.m_pObject;
			m_pLifeCount->lLifeCountRef++;

			m_is_data_dirty_ = i_otherPtr.m_is_data_dirty_;
			return *this;
		}
		else {
			releaseReferenCount();
			m_pLifeCount = nullptr;
			m_pObject = nullptr;
			m_is_data_dirty_ = true;
			return *this;
		}
	}

	inline bool operator > (const StrongPtr & i_otherPtr) {
		if(!i_otherPtr.m_is_data_dirty_)
			return this->m_pObject > i_otherPtr.m_pObject;
		else
			return this->m_pObject > nullptr;
	}
	inline bool operator < (const StrongPtr & i_otherPtr) {
		if (!i_otherPtr.m_is_data_dirty_)
			return this->m_pObject < i_otherPtr.m_pObject;
		else
			return this->m_pObject < nullptr;
	}
	inline bool operator == (const StrongPtr & i_otherPtr) {
		if (!i_otherPtr.m_is_data_dirty_)
			return this->m_pObject == i_otherPtr.m_pObject;
		else
			return this->m_pObject == nullptr;
	}
	inline bool operator != (const StrongPtr & i_otherPtr) {
		if (!i_otherPtr.m_is_data_dirty_)
			return this->m_pObject != i_otherPtr.m_pObject;
		else
			return this->m_pObject != nullptr;
	}
private:
	void releaseReferenCount() {
		if (!m_is_data_dirty_) {
			if ((--m_pLifeCount->lLifeCountRef) <= 0) {
				delete m_pObject;
				m_pObject = nullptr;

				if (m_pLifeCount->lObserverCountRef <= 0) {
					delete m_pLifeCount;
					m_pLifeCount = nullptr;
					m_is_data_dirty_ = true;
				}
			}
		}
	}
private:
	bool m_is_data_dirty_;
	T * m_pObject;
	LifeCount * m_pLifeCount;
};

template <class T>
class WeakPtr {
public:
	WeakPtr():m_is_data_dirty(true){
		;
	}
	WeakPtr(const StrongPtr<T>& i_strongPtr) {
		if (!i_strongPtr.m_is_data_dirty_) {
			m_pObjectObserved = i_strongPtr.m_pObject;
			m_pLifeCount = i_strongPtr.m_pLifeCount;
			m_pLifeCount->lObserverCountRef = 1;
			m_is_data_dirty = false;
		}
		else {
			m_pLifeCount = nullptr;
			m_pObjectObserved = nullptr;
			m_is_data_dirty = true;
		}
	}
	WeakPtr(const WeakPtr<T>& i_otherPtr) {
		if (!i_otherPtr.m_is_data_dirty) {
			m_pObjectObserved = i_otherPtr.m_pObjectObserved;
			m_pLifeCount = i_otherPtr.m_pLifeCount;
			m_pLifeCount->lObserverCountRef++;

			m_is_data_dirty = i_otherPtr.m_is_data_dirty;
		}
		else {
			m_pLifeCount = m_pObjectObserved = nullptr;
			m_is_data_dirty = true;
		}
	}
	~WeakPtr() {
		releaseReferenCount();
	}

	StrongPtr<T> Acquire() const {
		if (m_pLifeCount && m_pLifeCount->lLifeCountRef > 0) {
			return StrongPtr<T>(m_pObjectObserved, m_pLifeCount);
		}
		else {
			return StrongPtr<T>(nullptr);
		}
	}
public:
	WeakPtr<T> & operator= (const WeakPtr & i_otherPtr) {
		if (&i_otherPtr == this) {
			return *this;
		}
		if (!i_otherPtr.m_is_data_dirty) {
			releaseReferenCount();
			m_pObjectObserved = i_otherPtr.m_pObjectObserved;
			m_pLifeCount = i_otherPtr.m_pLifeCount;
			m_pLifeCount->lObserverCountRef++;
			m_is_data_dirty = false;
			return *this;
		}
		else {
			releaseReferenCount();
			m_pLifeCount = nullptr;
			m_pObjectObserved = nullptr;
			m_is_data_dirty = true;
			return *this;
		}
	}

	inline bool operator > (const WeakPtr & i_otherPtr) {
		if (!i_otherPtr.m_is_data_dirty)
			return this->m_pObject > i_otherPtr.m_pObject;
		else
			return this->m_pObject > nullptr;
	}
	inline bool operator < (const WeakPtr & i_otherPtr) {
		if (!i_otherPtr.m_is_data_dirty)
			return this->m_pObject < i_otherPtr.m_pObject;
		else
			return this->m_pObject < nullptr;
	}
	inline bool operator == (const WeakPtr & i_otherPtr) {
		if (!i_otherPtr.m_is_data_dirty)
			return this->m_pObject == i_otherPtr.m_pObject;
		else
			return this->m_pObject == nullptr;
	}
	inline bool operator != (const WeakPtr & i_otherPtr) {
		if (!i_otherPtr.m_is_data_dirty)
			return this->m_pObject != i_otherPtr.m_pObject;
		else
			return this->m_pObject != nullptr;
	}

private:
	void releaseReferenCount() {
		m_pLifeCount->lObserverCountRef--;
	}
private:
	T * m_pObjectObserved;
	LifeCount * m_pLifeCount;
	bool m_is_data_dirty;
};