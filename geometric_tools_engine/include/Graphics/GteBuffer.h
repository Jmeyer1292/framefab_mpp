// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2017
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 3.0.0 (2016/06/19)

#pragma once

#include <Graphics/GteResource.h>
#include <functional>

namespace gte
{

class GTE_IMPEXP Buffer : public Resource
{
protected:
    // Abstract base class.
    Buffer(unsigned int numElements, size_t elementSize,
        bool createStorage = true);
};

typedef std::function<void(std::shared_ptr<Buffer> const&)> BufferUpdater;

}
