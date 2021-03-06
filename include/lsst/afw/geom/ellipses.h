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

#ifndef LSST_AFW_GEOM_ELLIPSES_H
#define LSST_AFW_GEOM_ELLIPSES_H

/*
 *  Public header class for ellipse library.
 */

#include "lsst/afw/geom/ellipses/BaseCore.h"
#include "lsst/afw/geom/ellipses/Ellipse.h"
#include "lsst/afw/geom/ellipses/Transformer.h"
#include "lsst/afw/geom/ellipses/Convolution.h"
#include "lsst/afw/geom/ellipses/GridTransform.h"
#include "lsst/afw/geom/ellipses/Parametric.h"
#include "lsst/afw/geom/ellipses/radii.h"
#include "lsst/afw/geom/ellipses/Distortion.h"
#include "lsst/afw/geom/ellipses/ConformalShear.h"
#include "lsst/afw/geom/ellipses/ReducedShear.h"
#include "lsst/afw/geom/ellipses/Quadrupole.h"
#include "lsst/afw/geom/ellipses/Axes.h"
#include "lsst/afw/geom/ellipses/Separable.h"
#include "lsst/afw/geom/ellipses/PixelRegion.h"

namespace lsst {
namespace afw {
namespace geom {
namespace ellipses {

typedef Separable<Distortion, DeterminantRadius> SeparableDistortionDeterminantRadius;
typedef Separable<Distortion, TraceRadius> SeparableDistortionTraceRadius;
typedef Separable<Distortion, LogDeterminantRadius> SeparableDistortionLogDeterminantRadius;
typedef Separable<Distortion, LogTraceRadius> SeparableDistortionLogTraceRadius;

typedef Separable<ConformalShear, DeterminantRadius> SeparableConformalShearDeterminantRadius;
typedef Separable<ConformalShear, TraceRadius> SeparableConformalShearTraceRadius;
typedef Separable<ConformalShear, LogDeterminantRadius> SeparableConformalShearLogDeterminantRadius;
typedef Separable<ConformalShear, LogTraceRadius> SeparableConformalShearLogTraceRadius;

typedef Separable<ReducedShear, DeterminantRadius> SeparableReducedShearDeterminantRadius;
typedef Separable<ReducedShear, TraceRadius> SeparableReducedShearTraceRadius;
typedef Separable<ReducedShear, LogDeterminantRadius> SeparableReducedShearLogDeterminantRadius;
typedef Separable<ReducedShear, LogTraceRadius> SeparableReducedShearLogTraceRadius;
}  // namespace ellipses
}  // namespace geom
}  // namespace afw
}  // namespace lsst

#endif  // !LSST_AFW_GEOM_ELLIPSES_H
