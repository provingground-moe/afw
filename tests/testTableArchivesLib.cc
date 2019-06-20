/*
 * LSST Data Management System
 * Copyright 2008-2016  AURA/LSST.
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

#include <pybind11/pybind11.h>
//#include <pybind11/stl.h>

#include <memory>
#include "lsst/afw/cameraGeom.h"
#include "lsst/afw/detection.h"
#include "lsst/afw/image.h"
#include "lsst/afw/math.h"
#include "lsst/afw/table.h"
#include "lsst/afw/table/io/CatalogVector.h"
#include "lsst/afw/table/io/InputArchive.h"
#include "lsst/afw/table/io/OutputArchive.h"
#include "lsst/log/Log.h"

namespace py = pybind11;

// not really a Psf, just a Persistable we can stuff in an Exposure
class DummyPsf : public lsst::afw::detection::Psf {
public:
    virtual std::shared_ptr<lsst::afw::detection::Psf> clone() const {
        return std::shared_ptr<lsst::afw::detection::Psf>(new DummyPsf(_x));
    }

    virtual std::shared_ptr<lsst::afw::detection::Psf> resized(int width, int height) const {
        throw LSST_EXCEPT(lsst::pex::exceptions::LogicError, "Not Implemented");
    }

    virtual bool isPersistable() const noexcept { return true; }

    double getValue() const { return _x; }

    virtual lsst::geom::Box2I doComputeBBox(lsst::geom::Point2D const& position,
                                            lsst::afw::image::Color const& color) const {
        return lsst::geom::Box2I(lsst::geom::Point2I(-1, -1), lsst::geom::Point2I(1, 1));
    }

    explicit DummyPsf(double x) : _x(x) {}

protected:
    virtual std::shared_ptr<Image> doComputeKernelImage(lsst::geom::Point2D const& ccdXY,
                                                        lsst::afw::image::Color const& color) const {
        return std::shared_ptr<Image>();
    }

    virtual double doComputeApertureFlux(double radius, lsst::geom::Point2D const& ccdXY,
                                         lsst::afw::image::Color const& color) const {
        return 0.0;
    }

    virtual lsst::afw::geom::ellipses::Quadrupole doComputeShape(lsst::geom::Point2D const& ccdXY,
                                                                 lsst::afw::image::Color const& color) const {
        return lsst::afw::geom::ellipses::Quadrupole();
    }

    virtual std::string getPersistenceName() const { return "DummyPsf"; }

    virtual std::string getPythonModule() const { return "testTableArchivesLib"; }

    virtual void write(OutputArchiveHandle& handle) const;

    double _x;
};

namespace {

struct DummyPsfPersistenceHelper {
    lsst::afw::table::Schema schema;
    lsst::afw::table::Key<double> x;

    static DummyPsfPersistenceHelper const& get() {
        static DummyPsfPersistenceHelper instance;
        return instance;
    }

    // No copying
    DummyPsfPersistenceHelper(const DummyPsfPersistenceHelper&) = delete;
    DummyPsfPersistenceHelper& operator=(const DummyPsfPersistenceHelper&) = delete;

    // No moving
    DummyPsfPersistenceHelper(DummyPsfPersistenceHelper&&) = delete;
    DummyPsfPersistenceHelper& operator=(DummyPsfPersistenceHelper&&) = delete;

private:
    DummyPsfPersistenceHelper() : schema(), x(schema.addField<double>("x", "dummy parameter")) {
        schema.getCitizen().markPersistent();
    }
};

class DummyPsfFactory : public lsst::afw::table::io::PersistableFactory {
public:
    virtual std::shared_ptr<lsst::afw::table::io::Persistable> read(InputArchive const& archive,
                                                                    CatalogVector const& catalogs) const {
        static DummyPsfPersistenceHelper const& keys = DummyPsfPersistenceHelper::get();
        LSST_ARCHIVE_ASSERT(catalogs.size() == 1u);
        LSST_ARCHIVE_ASSERT(catalogs.front().size() == 1u);
        lsst::afw::table::BaseRecord const& record = catalogs.front().front();
        LSST_ARCHIVE_ASSERT(record.getSchema() == keys.schema);
        return std::make_shared<DummyPsf>(record.get(keys.x));
    }

    DummyPsfFactory(std::string const& name) : lsst::afw::table::io::PersistableFactory(name) {}
};

DummyPsfFactory registration("DummyPsf");

}  // namespace

void DummyPsf::write(OutputArchiveHandle& handle) const {
    static DummyPsfPersistenceHelper const& keys = DummyPsfPersistenceHelper::get();
    lsst::afw::table::BaseCatalog catalog = handle.makeCatalog(keys.schema);
    std::shared_ptr<lsst::afw::table::BaseRecord> record = catalog.addNew();
    (*record).set(keys.x, _x);
    handle.saveCatalog(catalog);
}

PYBIND11_DECLARE_HOLDER_TYPE(MyType, std::shared_ptr<MyType>);

PYBIND11_MODULE(testTableArchivesLib, mod) {
    py::module::import("lsst.afw.detection.psf");

    py::class_<DummyPsf, std::shared_ptr<DummyPsf>, lsst::afw::detection::Psf> cls(mod, "DummyPsf");

    cls.def(py::init<double>());

    cls.def("doComputeBBox", &DummyPsf::doComputeBBox);
    cls.def("clone", &DummyPsf::clone);
    cls.def("resized", &DummyPsf::resized);
    cls.def("isPersistable", &DummyPsf::isPersistable);
    cls.def("getValue", &DummyPsf::getValue);
    cls.def("__eq__",
            [](DummyPsf const& self, DummyPsf const& other) { return self.getValue() == other.getValue(); },
            py::is_operator());
}
