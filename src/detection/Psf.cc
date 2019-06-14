// -*- LSST-C++ -*-
#include <limits>
#include <typeinfo>
#include <cmath>
#include <memory>

#include "lsst/utils/Cache.h"
#include "lsst/afw/detection/Psf.h"
#include "lsst/afw/math/offsetImage.h"
#include "lsst/afw/table/io/Persistable.cc"

namespace lsst {
namespace afw {

template std::shared_ptr<detection::Psf> table::io::PersistableFacade<detection::Psf>::dynamicCast(
        std::shared_ptr<table::io::Persistable> const &);

namespace detection {
namespace detail {

// Key for caching PSFs with lsst::utils::Cache
//
// We cache PSFs by their x,y position. Although there are placeholders
// in the `Psf` class and here for `image::Color`, these are not used
// in the cache because `image::Color` is not currently well-defined
// or used.
struct PsfCacheKey {
    lsst::geom::Point2D const position;
    image::Color const color;

    PsfCacheKey(lsst::geom::Point2D const &position_, image::Color color_ = image::Color())
            : position(position_), color(color_) {}

    bool operator==(PsfCacheKey const &other) const {
        return position == other.position;  // Currently don't care about color
    }

    friend std::ostream &operator<<(std::ostream &os, PsfCacheKey const &key) { return os << key.position; }
};

}  // namespace detail
}  // namespace detection
}  // namespace afw
}  // namespace lsst

namespace std {

// Template specialisation for hashing PsfCacheKey
//
// We currently ignore the color, consistent with operator==.
template <>
struct hash<lsst::afw::detection::detail::PsfCacheKey> {
    using argument_type = lsst::afw::detection::detail::PsfCacheKey;
    using result_type = std::size_t;
    std::size_t operator()(lsst::afw::detection::detail::PsfCacheKey const &key) const noexcept {
        return std::hash<lsst::geom::Point2D>()(key.position);
    }
};

}  // namespace std

namespace lsst {
namespace afw {
namespace detection {

namespace {

bool isPointNull(lsst::geom::Point2D const &p) { return std::isnan(p.getX()) && std::isnan(p.getY()); }

}  // namespace

Psf::Psf(bool isFixed, std::size_t capacity) : _isFixed(isFixed) {
    _imageCache = std::make_unique<PsfCache>(capacity);
    _kernelImageCache = std::make_unique<PsfCache>(capacity);
}

Psf::~Psf() = default;

Psf::Psf(Psf const &other) : Psf(other._isFixed, other.getCacheCapacity()) {}

Psf::Psf(Psf &&other)
        : _isFixed(other._isFixed),
          _imageCache(std::move(other._imageCache)),
          _kernelImageCache(std::move(other._kernelImageCache)) {}

std::shared_ptr<image::Image<double>> Psf::recenterKernelImage(std::shared_ptr<Image> im,
                                                               lsst::geom::Point2D const &position,
                                                               std::string const &warpAlgorithm,
                                                               unsigned int warpBuffer) {
    // "ir" : (integer, residual)
    std::pair<int, double> const irX = image::positionToIndex(position.getX(), true);
    std::pair<int, double> const irY = image::positionToIndex(position.getY(), true);

    if (irX.second != 0.0 || irY.second != 0.0) {
        im = math::offsetImage(*im, irX.second, irY.second, warpAlgorithm, warpBuffer);
    }

    im->setXY0(irX.first + im->getX0(), irY.first + im->getY0());
    return im;
}

std::shared_ptr<Psf::Image> Psf::computeImage(lsst::geom::Point2D position, image::Color color,
                                              ImageOwnerEnum owner) const {
    if (isPointNull(position)) position = getAveragePosition();
    if (color.isIndeterminate()) color = getAverageColor();
    std::shared_ptr<Psf::Image> result = (*_imageCache)(
            detail::PsfCacheKey(position, color),
            [this](detail::PsfCacheKey const &key) { return doComputeImage(key.position, key.color); });
    if (owner == COPY) {
        result = std::make_shared<Image>(*result, true);
    }
    return result;
}

std::shared_ptr<Psf::Image> Psf::computeKernelImage(lsst::geom::Point2D position, image::Color color,
                                                    ImageOwnerEnum owner) const {
    if (_isFixed || isPointNull(position)) position = getAveragePosition();
    if (_isFixed || color.isIndeterminate()) color = getAverageColor();
    std::shared_ptr<Psf::Image> result = (*_kernelImageCache)(
            detail::PsfCacheKey(position, color),
            [this](detail::PsfCacheKey const &key) { return doComputeKernelImage(key.position, key.color); });
    if (owner == COPY) {
        result = std::make_shared<Image>(*result, true);
    }
    return result;
}

lsst::geom::Box2I Psf::computeBBox(lsst::geom::Point2D position, image::Color color) const {
    if (isPointNull(position)) position = getAveragePosition();
    if (color.isIndeterminate()) color = getAverageColor();
    return doComputeBBox(position, color);
}

std::shared_ptr<math::Kernel const> Psf::getLocalKernel(lsst::geom::Point2D position,
                                                        image::Color color) const {
    if (isPointNull(position)) position = getAveragePosition();
    if (color.isIndeterminate()) color = getAverageColor();
    // FixedKernel ctor will deep copy image, so we can use INTERNAL.
    std::shared_ptr<Image> image = computeKernelImage(position, color, INTERNAL);
    return std::make_shared<math::FixedKernel>(*image);
}

double Psf::computePeak(lsst::geom::Point2D position, image::Color color) const {
    if (isPointNull(position)) position = getAveragePosition();
    if (color.isIndeterminate()) color = getAverageColor();
    std::shared_ptr<Image> image = computeKernelImage(position, color, INTERNAL);
    return (*image)(-image->getX0(), -image->getY0());
}

double Psf::computeApertureFlux(double radius, lsst::geom::Point2D position, image::Color color) const {
    if (isPointNull(position)) position = getAveragePosition();
    if (color.isIndeterminate()) color = getAverageColor();
    return doComputeApertureFlux(radius, position, color);
}

geom::ellipses::Quadrupole Psf::computeShape(lsst::geom::Point2D position, image::Color color) const {
    if (isPointNull(position)) position = getAveragePosition();
    if (color.isIndeterminate()) color = getAverageColor();
    return doComputeShape(position, color);
}

std::shared_ptr<Psf::Image> Psf::doComputeImage(lsst::geom::Point2D const &position,
                                                image::Color const &color) const {
    std::shared_ptr<Psf::Image> im = computeKernelImage(position, color, COPY);
    return recenterKernelImage(im, position);
}

lsst::geom::Point2D Psf::getAveragePosition() const { return lsst::geom::Point2D(); }

std::size_t Psf::getCacheCapacity() const { return _kernelImageCache->capacity(); }

void Psf::setCacheCapacity(std::size_t capacity) {
    _imageCache->reserve(capacity);
    _kernelImageCache->reserve(capacity);
}

}  // namespace detection
}  // namespace afw
}  // namespace lsst
