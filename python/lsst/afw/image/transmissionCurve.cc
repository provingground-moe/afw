/*
 * LSST Data Management System
 * Copyright 2008-2017 AURA/LSST.
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
 * see <https://www.lsstcorp.org/LegalNotices/>.
 */

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <memory>

#include "ndarray/pybind11.h"

#include "lsst/afw/typehandling/Storable.h"
#include "lsst/afw/image/TransmissionCurve.h"
#include "lsst/afw/table/io/python.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace afw {
namespace image {
namespace {

using PyTransmissionCurve =
        py::class_<TransmissionCurve, std::shared_ptr<TransmissionCurve>, typehandling::Storable>;

PyTransmissionCurve declare(py::module & mod) {
    return PyTransmissionCurve(mod, "TransmissionCurve");
}

void define(PyTransmissionCurve & cls) {
    table::io::python::addPersistableMethods(cls);

    cls.def_static("makeIdentity", &TransmissionCurve::makeIdentity);
    cls.def_static("makeSpatiallyConstant", &TransmissionCurve::makeSpatiallyConstant,
                   "throughput"_a, "wavelengths"_a,
                   "throughputAtMin"_a=0.0, "throughputAtMax"_a=0.0);
    cls.def_static("makeRadial", &TransmissionCurve::makeRadial,
                   "throughput"_a, "wavelengths"_a, "radii"_a,
                   "throughputAtMin"_a=0.0, "throughputAtMax"_a=0.0);
    cls.def("__mul__", &TransmissionCurve::multipliedBy, py::is_operator());
    cls.def("multipliedBy", &TransmissionCurve::multipliedBy);
    cls.def("transformedBy", &TransmissionCurve::transformedBy, "transform"_a);
    cls.def("getWavelengthBounds", &TransmissionCurve::getWavelengthBounds);
    cls.def("getThroughputAtBounds", &TransmissionCurve::getThroughputAtBounds);
    cls.def(
        "sampleAt",
        (void (TransmissionCurve::*)(
            lsst::geom::Point2D const &,
            ndarray::Array<double const,1,1> const &,
            ndarray::Array<double,1,1> const &
        ) const) &TransmissionCurve::sampleAt,
        "position"_a, "wavelengths"_a, "out"_a
    );
    cls.def(
        "sampleAt",
        (ndarray::Array<double,1,1> (TransmissionCurve::*)(
            lsst::geom::Point2D const &,
            ndarray::Array<double const,1,1> const &
        ) const) &TransmissionCurve::sampleAt,
        "position"_a, "wavelengths"_a
    );
}

PYBIND11_MODULE(transmissionCurve, mod) {
    // then declare classes
    auto cls = declare(mod);
    // then import dependencies used in method signatures
    py::module::import("lsst.afw.geom");
    // and now we can safely define methods and other attributes
    define(cls);
}

}}}}  // namespace lsst::afw::image::<anonymous>

