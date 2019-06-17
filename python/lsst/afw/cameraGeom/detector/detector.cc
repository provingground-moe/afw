/*
 * LSST Data Management System
 * See COPYRIGHT file at the top of the source tree.
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

#include <memory>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ndarray/pybind11.h"

#include "lsst/afw/cameraGeom/CameraSys.h"
#include "lsst/afw/cameraGeom/Orientation.h"
#include "lsst/geom.h"
#include "lsst/afw/table/io/python.h"
#include "lsst/afw/cameraGeom/Detector.h"
#include "lsst/afw/cameraGeom/TransformMap.h"

namespace py = pybind11;
using namespace py::literals;

namespace lsst {
namespace afw {
namespace cameraGeom {

// Declare Detector methods overloaded on one coordinate system class
template <typename SysT, typename PyClass>
void declare1SysMethods(PyClass &cls) {
    cls.def("getCorners",
            (std::vector<lsst::geom::Point2D>(Detector::*)(SysT const &) const) & Detector::getCorners,
            "cameraSys"_a);
    cls.def("getCenter", (lsst::geom::Point2D(Detector::*)(SysT const &) const) & Detector::getCenter,
            "cameraSys"_a);
    cls.def("hasTransform", (bool (Detector::*)(SysT const &) const) & Detector::hasTransform, "cameraSys"_a);
    cls.def("makeCameraSys", (CameraSys const (Detector::*)(SysT const &) const) & Detector::makeCameraSys,
            "cameraSys"_a);
}

// Declare Detector methods templated on two coordinate system classes
template <typename FromSysT, typename ToSysT, typename PyClass>
void declare2SysMethods(PyClass &cls) {
    cls.def("getTransform",
            (std::shared_ptr<geom::TransformPoint2ToPoint2>(Detector::*)(FromSysT const &, ToSysT const &)
                     const) &
                    Detector::getTransform,
            "fromSys"_a, "toSys"_a);
    cls.def("transform",
            (lsst::geom::Point2D(Detector::*)(lsst::geom::Point2D const &, FromSysT const &, ToSysT const &) const) &
                    Detector::transform,
            "point"_a, "fromSys"_a, "toSys"_a);
    cls.def("transform",
            (std::vector<lsst::geom::Point2D>(Detector::*)(std::vector<lsst::geom::Point2D> const &, FromSysT const &,
                                                     ToSysT const &) const) &
                    Detector::transform,
            "points"_a, "fromSys"_a, "toSys"_a);
}

PYBIND11_MODULE(detector, mod) {
    /* Module level */
    py::class_<Detector, std::shared_ptr<Detector>> cls(mod, "Detector");

    /* Member types and enums */
    py::enum_<DetectorType>(mod, "DetectorType")
            .value("SCIENCE", DetectorType::SCIENCE)
            .value("FOCUS", DetectorType::FOCUS)
            .value("GUIDER", DetectorType::GUIDER)
            .value("WAVEFRONT", DetectorType::WAVEFRONT)
            .export_values();

    /* Constructors */
    cls.def(py::init<std::string const &, int, DetectorType, std::string const &, lsst::geom::Box2I const &,
                     std::vector<std::shared_ptr<Amplifier const>> const &, Orientation const &,
                     lsst::geom::Extent2D const &, TransformMap::Transforms const &,
                     Detector::CrosstalkMatrix const &, std::string const &>(),
            "name"_a, "id"_a, "type"_a, "serial"_a, "bbox"_a, "amplifiers"_a, "orientation"_a,
            "pixelSize"_a, "transforms"_a, "crosstalk"_a = Detector::CrosstalkMatrix(),
            "physicalType"_a = "");
    cls.def(py::init<std::string const &, int, DetectorType, std::string const &, lsst::geom::Box2I const &,
                     std::vector<std::shared_ptr<Amplifier const>> const &, Orientation const &,
                     lsst::geom::Extent2D const &, std::shared_ptr<TransformMap const>,
                     Detector::CrosstalkMatrix const &, std::string const &>(),
            "name"_a, "id"_a, "type"_a, "serial"_a, "bbox"_a, "amplifiers"_a, "orientation"_a,
            "pixelSize"_a, "transformMap"_a, "crosstalk"_a = Detector::CrosstalkMatrix(),
            "physicalType"_a = "");

    /* Operators */
    cls.def("__getitem__",
            (std::shared_ptr<Amplifier const>(Detector::*)(int) const) & Detector::_get, "i"_a);
    cls.def("__getitem__",
            (std::shared_ptr<Amplifier const>(Detector::*)(std::string const &) const) &
                    Detector::_get,
            "name"_a);
    cls.def("__len__", &Detector::size);

    /* Members */
    cls.def("getName", &Detector::getName);
    cls.def("getId", &Detector::getId);
    cls.def("getType", &Detector::getType);
    cls.def("getPhysicalType", &Detector::getPhysicalType);
    cls.def("getSerial", &Detector::getSerial);
    cls.def("getBBox", &Detector::getBBox);
    cls.def("getAmpInfoCatalog", &Detector::getAmplifiers);  // TODO: deprecate this in Python
    cls.def("getAmplifiers", &Detector::getAmplifiers);
    cls.def("getOrientation", &Detector::getOrientation);
    cls.def("getPixelSize", &Detector::getPixelSize);
    cls.def("hasCrosstalk", &Detector::hasCrosstalk);
    cls.def("getCrosstalk", &Detector::getCrosstalk);
    cls.def("getTransformMap", &Detector::getTransformMap);
    cls.def("getNativeCoordSys", &Detector::getNativeCoordSys);
    declare1SysMethods<CameraSys>(cls);
    declare1SysMethods<CameraSysPrefix>(cls);
    declare2SysMethods<CameraSys, CameraSys>(cls);
    declare2SysMethods<CameraSys, CameraSysPrefix>(cls);
    declare2SysMethods<CameraSysPrefix, CameraSys>(cls);
    declare2SysMethods<CameraSysPrefix, CameraSysPrefix>(cls);

    table::io::python::addPersistableMethods(cls);
}
}  // namespace cameraGeom
}  // namespace afw
}  // namespace lsst
