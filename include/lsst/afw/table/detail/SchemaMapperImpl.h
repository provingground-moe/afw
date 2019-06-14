// -*- lsst-c++ -*-
#ifndef AFW_TABLE_DETAIL_SchemaMapperImpl_h_INCLUDED
#define AFW_TABLE_DETAIL_SchemaMapperImpl_h_INCLUDED

#include <map>
#include <algorithm>

#include "boost/variant.hpp"
#include "boost/mpl/transform.hpp"

#include "lsst/afw/table/Schema.h"

namespace lsst {
namespace afw {
namespace table {

class SchemaMapper;

namespace detail {

/**
 * A private implementation class to hide the messy details of SchemaMapper.
 *
 * This class is very similar in spirit to SchemaImpl; look there for more information (though SchemaMapper
 * is not copy-on-write).
 */
class SchemaMapperImpl final {
private:
    /// Boost.MPL metafunction that returns a std::pair< Key<T>, Key<T> > given a T.
    struct MakeKeyPair {
        template <typename T>
        struct apply {
            typedef std::pair<Key<T>, Key<T> > type;
        };
    };

public:
    /// An MPL sequence of all the allowed pair templates.
    typedef boost::mpl::transform<FieldTypes, MakeKeyPair>::type KeyPairTypes;
    /// A Boost.Variant type that can hold any one of the allowed pair types.
    typedef boost::make_variant_over<KeyPairTypes>::type KeyPairVariant;
    /// A std::vector whose elements can be any of the allowed pair types.
    typedef std::vector<KeyPairVariant> KeyPairMap;

    /// Constructor from the given input and output schemas
    explicit SchemaMapperImpl(Schema const& input, Schema const& output) : _input(input), _output(output) {}

    /**
     *  A functor-wrapper used in the implementation of SchemaMapper::forEach.
     *
     *  See SchemaImpl::VisitorWrapper for discussion of the motivation.
     */
    template <typename F>
    struct VisitorWrapper : public boost::static_visitor<> {
        /// Call the wrapped function.
        template <typename T>
        void operator()(std::pair<Key<T>, Key<T> > const& pair) const {
            _func(pair.first, pair.second);
        }

        /**
         *  Invoke the visitation.
         *
         *  The call to boost::apply_visitor will call the appropriate template of operator().
         *
         *  This overload allows a VisitorWrapper to be applied directly on a variant object
         *  with function-call syntax, allowing us to use it on our vector of variants with
         *  std::for_each and other STL algorithms.
         */
        void operator()(KeyPairVariant const& v) const { boost::apply_visitor(*this, v); }

        /// Construct the wrappper.
        template <typename T>
        explicit VisitorWrapper(T&& func) : _func(std::forward<T>(func)) {}

    private:
        F _func;
    };

private:
    friend class table::SchemaMapper;
    friend class detail::Access;

    Schema _input;
    Schema _output;
    KeyPairMap _map;
};
}  // namespace detail
}  // namespace table
}  // namespace afw
}  // namespace lsst

#endif  // !AFW_TABLE_DETAIL_SchemaMapperImpl_h_INCLUDED
