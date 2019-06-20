// -*- LSST-C++ -*- // fixed format comment for emacs
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

#include "lsst/pex/exceptions.h"
#include "lsst/log/Log.h"
#include "lsst/afw/image/ExposureInfo.h"
#include "lsst/afw/image/PhotoCalib.h"
#include "lsst/afw/geom/polygon/Polygon.h"
#include "lsst/afw/geom/SkyWcs.h"
#include "lsst/afw/image/ApCorrMap.h"
#include "lsst/afw/detection/Psf.h"
#include "lsst/afw/cameraGeom/Detector.h"
#include "lsst/afw/image/TransmissionCurve.h"
#include "lsst/afw/fits.h"
#include "lsst/afw/typehandling/SimpleGenericMap.h"

namespace {
LOG_LOGGER _log = LOG_GET("afw.image.ExposureInfo");
}  // namespace

namespace lsst {
namespace afw {
namespace image {

int ExposureInfo::getFitsSerializationVersion() {
    // Version history:
    // unversioned and 0: photometric calibration via Calib, WCS via SkyWcs using AST.
    // 1: photometric calibration via PhotoCalib
    static int const version = 1;
    return version;
}

std::string const& ExposureInfo::getFitsSerializationVersionName() {
    static std::string const versionName("EXPINFO_V");
    return versionName;
}

// Clone various components; defined here so that we don't have to expose their insides in Exposure.h

std::shared_ptr<ApCorrMap> ExposureInfo::_cloneApCorrMap(std::shared_ptr<ApCorrMap const> apCorrMap) {
    if (apCorrMap) {
        return std::make_shared<ApCorrMap>(*apCorrMap);
    }
    return std::shared_ptr<ApCorrMap>();
}

ExposureInfo::ExposureInfo(std::shared_ptr<geom::SkyWcs const> const& wcs,
                           std::shared_ptr<detection::Psf const> const& psf,
                           std::shared_ptr<PhotoCalib const> const& photoCalib,
                           std::shared_ptr<cameraGeom::Detector const> const& detector,
                           std::shared_ptr<geom::polygon::Polygon const> const& polygon, Filter const& filter,
                           std::shared_ptr<daf::base::PropertySet> const& metadata,
                           std::shared_ptr<CoaddInputs> const& coaddInputs,
                           std::shared_ptr<ApCorrMap> const& apCorrMap,
                           std::shared_ptr<image::VisitInfo const> const& visitInfo,
                           std::shared_ptr<TransmissionCurve const> const& transmissionCurve)
        : _wcs(wcs),
          _psf(std::const_pointer_cast<detection::Psf>(psf)),
          _photoCalib(photoCalib),
          _detector(detector),
          _validPolygon(polygon),
          _filter(filter),
          _metadata(metadata ? metadata
                             : std::shared_ptr<daf::base::PropertySet>(new daf::base::PropertyList())),
          _coaddInputs(coaddInputs),
          _apCorrMap(_cloneApCorrMap(apCorrMap)),
          _visitInfo(visitInfo),
          _transmissionCurve(transmissionCurve),
          _components(std::make_shared<typehandling::SimpleGenericMap<std::string>>()) {}

ExposureInfo::ExposureInfo(ExposureInfo const& other)
        : _wcs(other._wcs),
          _psf(other._psf),
          _photoCalib(other._photoCalib),
          _detector(other._detector),
          _validPolygon(other._validPolygon),
          _filter(other._filter),
          _metadata(other._metadata),
          _coaddInputs(other._coaddInputs),
          _apCorrMap(_cloneApCorrMap(other._apCorrMap)),
          _visitInfo(other._visitInfo),
          _transmissionCurve(other._transmissionCurve),
          _components(other._components) {}

// Delegate to copy-constructor for backwards compatibility
ExposureInfo::ExposureInfo(ExposureInfo&& other) : ExposureInfo(other) {}

ExposureInfo::ExposureInfo(ExposureInfo const& other, bool copyMetadata)
        : _wcs(other._wcs),
          _psf(other._psf),
          _photoCalib(other._photoCalib),
          _detector(other._detector),
          _validPolygon(other._validPolygon),
          _filter(other._filter),
          _metadata(other._metadata),
          _coaddInputs(other._coaddInputs),
          _apCorrMap(_cloneApCorrMap(other._apCorrMap)),
          _visitInfo(other._visitInfo),
          _transmissionCurve(other._transmissionCurve),
          _components(other._components) {
    if (copyMetadata) _metadata = _metadata->deepCopy();
}

ExposureInfo& ExposureInfo::operator=(ExposureInfo const& other) {
    if (&other != this) {
        _wcs = other._wcs;
        _psf = other._psf;
        _photoCalib = other._photoCalib;
        _detector = other._detector;
        _validPolygon = other._validPolygon;
        _filter = other._filter;
        _metadata = other._metadata;
        _coaddInputs = other._coaddInputs;
        _apCorrMap = _cloneApCorrMap(other._apCorrMap);
        _visitInfo = other._visitInfo;
        _transmissionCurve = other._transmissionCurve;
        _components = other._components;
    }
    return *this;
}
// Delegate to copy-assignment for backwards compatibility
ExposureInfo& ExposureInfo::operator=(ExposureInfo&& other) { return *this = other; }

void ExposureInfo::initApCorrMap() { _apCorrMap = std::make_shared<ApCorrMap>(); }

ExposureInfo::~ExposureInfo() = default;

int ExposureInfo::_addToArchive(FitsWriteData& data, table::io::Persistable const& object, std::string key,
                                std::string comment) {
    int componentId = data.archive.put(object);
    data.metadata->set(key, componentId, comment);
    return componentId;
}

int ExposureInfo::_addToArchive(FitsWriteData& data,
                                std::shared_ptr<table::io::Persistable const> const& object, std::string key,
                                std::string comment) {
    // Don't delegate to Persistable const& version because OutputArchive::put
    // has special handling of shared_ptr
    int componentId = data.archive.put(object);
    data.metadata->set(key, componentId, comment);
    return componentId;
}

ExposureInfo::FitsWriteData ExposureInfo::_startWriteFits(lsst::geom::Point2I const& xy0) const {
    FitsWriteData data;

    data.metadata.reset(new daf::base::PropertyList());
    data.imageMetadata.reset(new daf::base::PropertyList());
    data.maskMetadata = data.imageMetadata;
    data.varianceMetadata = data.imageMetadata;

    data.metadata->combine(getMetadata());

    data.metadata->set(getFitsSerializationVersionName(), getFitsSerializationVersion());

    // In the future, we might not have exactly three image HDUs, but we always do right now,
    // so 0=primary, 1=image, 2=mask, 3=variance, 4+=archive
    //
    // Historically the AR_HDU keyword was 1-indexed (see RFC-304), and to maintain file compatibility
    // this is still the case so we're setting AR_HDU to 5 == 4 + 1
    //
    data.metadata->set("AR_HDU", 5, "HDU (1-indexed) containing the archive used to store ancillary objects");
    if (hasCoaddInputs()) {
        _addToArchive(data, getCoaddInputs(), "COADD_INPUTS_ID", "archive ID for coadd inputs catalogs");
    }
    if (hasApCorrMap()) {
        _addToArchive(data, getApCorrMap(), "AP_CORR_MAP_ID", "archive ID for aperture correction map");
    }
    if (hasPsf() && getPsf()->isPersistable()) {
        _addToArchive(data, getPsf(), "PSF_ID", "archive ID for the Exposure's main Psf");
    }
    if (hasWcs() && getWcs()->isPersistable()) {
        _addToArchive(data, getWcs(), "SKYWCS_ID", "archive ID for the Exposure's main Wcs");
    }
    if (hasValidPolygon() && getValidPolygon()->isPersistable()) {
        _addToArchive(data, getValidPolygon(), "VALID_POLYGON_ID",
                      "archive ID for the Exposure's valid polygon");
    }
    if (hasTransmissionCurve() && getTransmissionCurve()->isPersistable()) {
        _addToArchive(data, getTransmissionCurve(), "TRANSMISSION_CURVE_ID",
                      "archive ID for the Exposure's transmission curve");
    }
    if (hasDetector() && getDetector()->isPersistable()) {
        _addToArchive(data, getDetector(), "DETECTOR_ID", "archive ID for the Exposure's Detector");
    }
    if (hasPhotoCalib()) {
        _addToArchive(data, getPhotoCalib(), "PHOTOCALIB_ID", "archive ID for photometric calibration");
    }

    // LSST convention is that Wcs is in pixel coordinates (i.e relative to bottom left
    // corner of parent image, if any). The Wcs/Fits convention is that the Wcs is in
    // image coordinates. When saving an image we convert from pixel to index coordinates.
    // In the case where this image is a parent image, the reference pixels are unchanged
    // by this transformation
    if (hasWcs()) {
        // Try to save the WCS as FITS-WCS metadata; if an exact representation
        // is not possible then skip it
        auto shift = lsst::geom::Extent2D(lsst::geom::Point2I(0, 0) - xy0);
        auto newWcs = getWcs()->copyAtShiftedPixelOrigin(shift);
        std::shared_ptr<daf::base::PropertyList> wcsMetadata;
        try {
            wcsMetadata = newWcs->getFitsMetadata(true);
        } catch (pex::exceptions::RuntimeError) {
            // cannot represent this WCS as FITS-WCS; don't write its metadata
        }
        if (wcsMetadata) {
            data.imageMetadata->combine(newWcs->getFitsMetadata(true));
        }
    }

    // For the sake of ds9, store _x0 and _y0 as -LTV1, -LTV2.
    // This is in addition to saving _x0 and _y0 as WCS A, which is done elsewhere
    // and is what LSST uses to read _x0 and _y0.
    // LTV is a convention used by STScI (see \S2.6.2 of HST Data Handbook for STIS, version 5.0
    // http://www.stsci.edu/hst/stis/documents/handbooks/currentDHB/ch2_stis_data7.html#429287)
    // and recognized by ds9.
    data.imageMetadata->set("LTV1", static_cast<double>(-xy0.getX()));
    data.imageMetadata->set("LTV2", static_cast<double>(-xy0.getY()));

    data.metadata->set("FILTER", getFilter().getName());
    if (hasDetector()) {
        data.metadata->set("DETNAME", getDetector()->getName());
        data.metadata->set("DETSER", getDetector()->getSerial());
    }

    auto visitInfoPtr = getVisitInfo();
    if (visitInfoPtr) {
        detail::setVisitInfoMetadata(*(data.metadata), *visitInfoPtr);
    }

    return data;
}

void ExposureInfo::_finishWriteFits(fits::Fits& fitsfile, FitsWriteData const& data) const {
    data.archive.writeFits(fitsfile);
}

}  // namespace image
}  // namespace afw
}  // namespace lsst
