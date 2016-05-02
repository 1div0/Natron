/* ***** BEGIN LICENSE BLOCK *****
 * This file is part of Natron <http://www.natron.fr/>,
 * Copyright (C) 2016 INRIA and Alexandre Gauthier-Foichat
 *
 * Natron is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Natron is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Natron.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
 * ***** END LICENSE BLOCK ***** */

#ifndef IMAGELOCKER_H
#define IMAGELOCKER_H

// ***** BEGIN PYTHON BLOCK *****
// from <https://docs.python.org/3/c-api/intro.html#include-files>:
// "Since Python may define some pre-processor definitions which affect the standard headers on some systems, you must include Python.h before any standard headers are included."
#include <Python.h>
// ***** END PYTHON BLOCK *****

#include <cassert>
#include <list>

#if !defined(Q_MOC_RUN) && !defined(SBK_RUN)
#include <boost/shared_ptr.hpp>
#endif


#include "Global/Macros.h"
#include "Engine/EngineFwd.h"

NATRON_NAMESPACE_ENTER;

template<typename EntryType>
class LockManagerI
{
public:

    LockManagerI() {}

    virtual ~LockManagerI() {}

    virtual void lock(const boost::shared_ptr<EntryType>& entry) = 0;
    virtual bool tryLock(const boost::shared_ptr<EntryType>& entry) = 0;
    virtual void unlock(const boost::shared_ptr<EntryType>& entry) = 0;
};

/**
 * @brief Small interface describing an image locker.
 * The idea is that the cache will create a new entry
 * and call lock(entry) on it because its memory has not
 * yet been initialized via CacheEntry::allocateMemory()
 **/
template<typename EntryType>
class ImageLockerHelper
{
    LockManagerI<EntryType>* _manager;
    std::list<boost::shared_ptr<EntryType> > _entries;

public:


    ImageLockerHelper(LockManagerI<EntryType>* manager) : _manager(manager), _entries() {}

    ImageLockerHelper(LockManagerI<EntryType>* manager,
                      const boost::shared_ptr<EntryType>& entry) : _manager(manager), _entries()
    {
        if (entry && _manager) {
            _entries.push_back(entry);
            _manager->lock(entry);
        }
    }

    ~ImageLockerHelper()
    {
        unlock();
    }

    void lock(const boost::shared_ptr<EntryType>& entry)
    {
        _entries.push_back(entry);
        if (_manager) {
            _manager->lock(entry);
        }
    }

    bool tryLock(const boost::shared_ptr<EntryType>& entry)
    {
        if (_manager) {
            if ( _manager->tryLock(entry) ) {
                _entries.push_back(entry);

                return true;
            }
        }

        return false;
    }

    void unlock()
    {
        if (!_entries.empty() && _manager) {
            for (typename std::list<boost::shared_ptr<EntryType> >::iterator it = _entries.begin(); it!=_entries.end(); ++it) {
                _manager->unlock(*it);
            }
            
            _entries.clear();
        }
    }
};

typedef ImageLockerHelper<Image> ImageLocker;
typedef ImageLockerHelper<FrameEntry> FrameEntryLocker;

NATRON_NAMESPACE_EXIT;

#endif // IMAGELOCKER_H
