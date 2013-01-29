// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
%define detailLib_DOCSTRING
"
Python interface to lsst::afw::math::detail classes and functions
"
%enddef

%feature("autodoc", "1");
%module(package="lsst.afw.math.detail", docstring=detailLib_DOCSTRING) detailLib

%{
#include "lsst/afw/image/Image.h"
#include "lsst/afw/image/Mask.h"
#include "lsst/afw/image/MaskedImage.h"
%}

%include "lsst/p_lsstSwig.i"

%import "lsst/afw/image/Image.i"
%import "lsst/afw/image/Mask.i"
%import "lsst/afw/image/MaskedImage.i"
%import "lsst/afw/math/kernel.i"

%lsst_exceptions();

%include "convolve.i"
