/*
    Copyright 2007-2008 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_READ_AND_CONVERT_VIEW_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_IO_READ_AND_CONVERT_VIEW_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/mpl/and.hpp>
#include <boost/utility/enable_if.hpp>

#include "base.hpp"
#include "io_device.hpp"
#include "path_spec.hpp"
#include "conversion_policies.hpp"

namespace boost
{
namespace gil
{

// ----- View --- Reader --------- With ColorConverter -----------------


/// \ingroup IO
template< typename Device
, typename View
, typename ColorConverter
, typename FormatTag
>
inline
void read_and_convert_view( Device&                                 file
                            , const View&                             view
                            , const image_read_settings< FormatTag >& settings
                            , const ColorConverter&                   cc
                            , typename enable_if< mpl::and_< is_format_tag< FormatTag >
                            , detail::is_input_device< Device >
                            >
                            >::type* ptr = 0
                          )
{
    typedef detail::read_and_convert< ColorConverter > reader_color_convert;

    detail::reader< Device
    , FormatTag
    , reader_color_convert
    > reader( file
              , cc
            );

    reader.init_view( view
                      , settings
                      , reader.get_info()
                    );

    reader.apply( view );
}

template< typename Device
, typename View
, typename ColorConverter
, typename FormatTag
>
inline
void read_and_convert_view( Device&                                 file
                            , const View&                             view
                            , const image_read_settings< FormatTag >& settings
                            , const ColorConverter&                   cc
                            , typename enable_if< mpl::and_< detail::is_adaptable_input_device< FormatTag
                            , Device
                            >
                            , is_format_tag< FormatTag >
                            >
                            >::type* ptr = 0
                          )
{
    typedef typename detail::is_adaptable_input_device< FormatTag
    , Device
    >::device_type device_type;

    device_type dev( file );

    read_and_convert_view( dev
                           , view
                           , settings
                           , cc
                         );
}

template < typename String
, typename View
, typename ColorConverter
, typename FormatTag
>
inline
void read_and_convert_view( const String&                           file_name
                            , const View&                             view
                            , const image_read_settings< FormatTag >& settings
                            , const ColorConverter&                   cc
                            , typename enable_if< mpl::and_< is_format_tag< FormatTag >
                            , detail::is_supported_path_spec< String >
                            >
                            >::type* ptr = 0
                          )
{
    detail::file_stream_device< FormatTag > device( detail::convert_to_string( file_name )
            , typename detail::file_stream_device< FormatTag >::read_tag()
                                                  );

    read_and_convert_view( device
                           , view
                           , settings
                           , cc
                         );
}

/// \ingroup IO
template < typename String
, typename View
, typename ColorConverter
, typename FormatTag
>
inline
void read_and_convert_view( const String&         file_name
                            , const View&           view
                            , const ColorConverter& cc
                            , const FormatTag&      tag
                            , typename enable_if< mpl::and_< is_format_tag< FormatTag >
                            , detail::is_supported_path_spec< String >
                            >
                            >::type* ptr = 0
                          )
{
    read_and_convert_view( file_name
                           , view
                           , image_read_settings< FormatTag >()
                           , cc
                         );
}

template < typename Device
, typename View
, typename ColorConverter
, typename FormatTag
>
inline
void read_and_convert_view( Device&               device
                            , const View&           view
                            , const ColorConverter& cc
                            , const FormatTag&      tag
                            , typename enable_if< mpl::and_< mpl::or_< detail::is_input_device< Device >
                            , detail::is_adaptable_input_device< FormatTag
                            , Device
                            >
                            >
                            , is_format_tag< FormatTag >
                            >
                            >::type* ptr = 0
                          )
{
    read_and_convert_view( device
                           , view
                           , image_read_settings< FormatTag >()
                           , cc
                         );
}

/// \ingroup IO
template < typename String
, typename View
, typename FormatTag
>
inline
void read_and_convert_view( const String&                           file_name
                            , const View&                             view
                            , const image_read_settings< FormatTag >& settings
                            , typename enable_if< mpl::and_< is_format_tag< FormatTag >
                            , detail::is_supported_path_spec< String >
                            >
                            >::type* ptr = 0
                          )
{
    read_and_convert_view( file_name
                           , view
                           , settings
                           , default_color_converter()
                         );
}


template < typename Device
, typename View
, typename FormatTag
>
inline
void read_and_convert_view( Device&                                 device
                            , const View&                             view
                            , const image_read_settings< FormatTag >& settings
                            , typename enable_if< mpl::and_< mpl::or_< detail::is_input_device< Device >
                            , detail::is_adaptable_input_device< FormatTag
                            , Device
                            >
                            >
                            , is_format_tag< FormatTag >
                            >
                            >::type* ptr = 0
                          )
{
    read_and_convert_view( device
                           , view
                           , settings
                           , default_color_converter()
                         );
}

/// \ingroup IO
template < typename String
, typename View
, typename FormatTag
>
inline
void read_and_convert_view( const String&    file_name
                            , const View&      view
                            , const FormatTag& tag
                            , typename enable_if< mpl::and_< is_format_tag< FormatTag >
                            , detail::is_supported_path_spec< String >
                            >
                            >::type* ptr = 0
                          )
{
    read_and_convert_view( file_name
                           , view
                           , image_read_settings< FormatTag >()
                           , default_color_converter()
                         );
}

template < typename Device
, typename View
, typename FormatTag
>
inline
void read_and_convert_view( Device&          device
                            , const View&      view
                            , const FormatTag& tag
                            , typename enable_if< mpl::and_< mpl::or_< detail::is_input_device< Device >
                            , detail::is_adaptable_input_device< FormatTag
                            , Device
                            >
                            >
                            , is_format_tag< FormatTag >
                            >
                            >::type* ptr = 0
                          )
{
    read_and_convert_view( device
                           , view
                           , image_read_settings< FormatTag >()
                           , default_color_converter()
                         );
}

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_READ_AND_CONVERT_VIEW_HPP_INCLUDED
