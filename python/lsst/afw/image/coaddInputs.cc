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

#include "lsst/afw/table/io/python.h"  // for addPersistableMethods
#include "lsst/afw/table/Schema.h"
#include "lsst/afw/table/Exposure.h"
#include "lsst/afw/typehandling/Storable.h"
#include "lsst/afw/image/CoaddInputs.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace afw {
namespace image {
namespace {

using PyCoaddInputs = py::class_<CoaddInputs, std::shared_ptr<CoaddInputs>, typehandling::Storable>;

PYBIND11_MODULE(coaddInputs, mod) {
    /* Module level */

    PyCoaddInputs cls(mod, "CoaddInputs");

    /* Constructors */
    cls.def(py::init<>());
    cls.def(py::init<table::Schema const &, table::Schema const &>(), "visitSchema"_a, "ccdSchema"_a);
    cls.def(py::init<table::ExposureCatalog const &, table::ExposureCatalog const &>(), "visits"_a, "ccds"_a);

    table::io::python::addPersistableMethods<CoaddInputs>(cls);

    /* Members */
    cls.def_readwrite("visits", &CoaddInputs::visits);
    cls.def_readwrite("ccds", &CoaddInputs::ccds);
    cls.def("isPersistable", &CoaddInputs::isPersistable);
}
}
}
}
}  // namespace lsst::afw::image::<anonymous>
