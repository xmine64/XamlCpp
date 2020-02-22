#ifndef XAML_UI_DRAWING_HPP
#define XAML_UI_DRAWING_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <sstream>
#include <tuple>
#include <utility>
#include <xaml/meta/conv.hpp>
#include <xaml/strings.hpp>

#ifdef XAML_UI_WINDOWS
#include <Windows.h>
#include <gdiplus.h>
#elif defined(XAML_UI_GTK3)
#include <gtk/gtk.h>
#elif defined(XAML_UI_COCOA)
#include <xaml/ui/objc.hpp>
#endif // XAML_UI_WINDOWS

namespace xaml
{
    template <typename T, typename TTuple, std::size_t... Indicies>
    T __intialize_from_tuple_impl(TTuple&& t, std::index_sequence<Indicies...>)
    {
        return T{ std::get<Indicies>(std::forward<TTuple>(t))... };
    }

    template <typename T, typename TTuple>
    T __initialize_from_tuple(TTuple&& t)
    {
        return __intialize_from_tuple_impl<T>(std::forward<TTuple>(t), std::make_index_sequence<std::tuple_size_v<TTuple>>{});
    }

    template <typename T, typename TTuple, TTuple (*func)(std::string_view), TTuple (*wfunc)(std::wstring_view)>
    struct __tuple___value_converter_traits_helper
    {
        static TTuple __convert(std::any value)
        {
            if (value.type() == typeid(TTuple))
            {
                return std::any_cast<TTuple>(value);
            }
            else if (value.type() == typeid(std::string))
            {
                return func(std::any_cast<std::string>(value));
            }
            else if (value.type() == typeid(std::string_view))
            {
                return func(std::any_cast<std::string_view>(value));
            }
            else if (value.type() == typeid(char*) || value.type() == typeid(const char*))
            {
                return func(std::any_cast<const char*>(value));
            }
            else if (value.type() == typeid(std::wstring))
            {
                return wfunc(std::any_cast<std::wstring>(value));
            }
            else if (value.type() == typeid(std::wstring_view))
            {
                return wfunc(std::any_cast<std::wstring_view>(value));
            }
            else if (value.type() == typeid(wchar_t*) || value.type() == typeid(const wchar_t*))
            {
                return wfunc(std::any_cast<const wchar_t*>(value));
            }
            else
            {
                return {};
            }
        }

        static T convert(std::any value)
        {
            if (value.type() == typeid(T))
            {
                return std::any_cast<T>(value);
            }
            else
            {
                auto t = __convert(std::move(value));
                return __initialize_from_tuple<T>(std::move(t));
            }
        }
    };

    STRING_CONST(__delimeter, " ,\t\r\n")

    struct size
    {
        double width;
        double height;
    };

    constexpr bool operator==(size lhs, size rhs) { return lhs.width == rhs.width && lhs.height == rhs.height; }
    constexpr bool operator!=(size lhs, size rhs) { return !(lhs == rhs); }

    constexpr size operator*(size lhs, double rhs) { return { lhs.width * rhs, lhs.height * rhs }; }
    constexpr size operator*(double lhs, size rhs) { return rhs * lhs; }
    constexpr size operator/(size lhs, double rhs) { return lhs * (1 / rhs); }

    template <typename To, typename From>
    inline To to_native(From) = delete;

#ifdef XAML_UI_WINDOWS
    constexpr size from_native(SIZE s)
    {
        return { (double)s.cx, (double)s.cy };
    }
    inline size from_native(Gdiplus::SizeF s)
    {
        return { (double)s.Width, (double)s.Height };
    }
    template <>
    constexpr SIZE to_native<SIZE, size>(size s)
    {
        return { (LONG)s.width, (LONG)s.height };
    }
    template <>
    inline Gdiplus::SizeF to_native<Gdiplus::SizeF, size>(size s)
    {
        return { (float)s.width, (float)s.height };
    }
#elif defined(XAML_UI_GTK3)
    constexpr size from_native(gint width, gint height)
    {
        return { (double)width, (double)height };
    }
    template <>
    inline std::tuple<gint, gint> to_native<std::tuple<gint, gint>, size>(size s)
    {
        return std::make_tuple((std::max)(0, (int)s.width), (std::max)(1, (int)s.height));
    }
#elif defined(XAML_UI_COCOA) && defined(__OBJC__)
    constexpr size from_native(NSSize s)
    {
        return { s.width, s.height };
    }
    template <>
    constexpr NSSize to_native<NSSize, size>(size s)
    {
        return { s.width, s.height };
    }
#endif // XAML_UI_WINDOWS

    struct point
    {
        double x;
        double y;
    };

    constexpr bool operator==(point lhs, point rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
    constexpr bool operator!=(point lhs, point rhs) { return !(lhs == rhs); }

    constexpr point operator+(point lhs, point rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
    constexpr point operator-(point lhs, point rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y }; }

    constexpr point operator*(point lhs, double rhs) { return { lhs.x * rhs, lhs.y * rhs }; }
    constexpr point operator*(double lhs, point rhs) { return rhs * lhs; }
    constexpr point operator/(point lhs, double rhs) { return lhs * (1 / rhs); }

#ifdef XAML_UI_WINDOWS
    constexpr point from_native(POINT p)
    {
        return { (double)p.x, (double)p.y };
    }
    inline point from_native(Gdiplus::PointF p)
    {
        return { (double)p.X, (double)p.Y };
    }
    template <>
    constexpr POINT to_native<POINT, point>(point p)
    {
        return { (LONG)p.x, (LONG)p.y };
    }
    template <>
    inline Gdiplus::PointF to_native<Gdiplus::PointF, point>(point p)
    {
        return { (float)p.x, (float)p.y };
    }
#elif defined(XAML_UI_GTK3)
    constexpr point from_native(GdkPoint p)
    {
        return { (double)p.x, (double)p.y };
    }
    template <>
    constexpr GdkPoint to_native<GdkPoint, point>(point p)
    {
        return { (int)p.x, (int)p.y };
    }
#elif defined(XAML_UI_COCOA) && defined(__OBJC__)
    constexpr point from_native(NSPoint p)
    {
        return { p.x, p.y };
    }
    template <>
    constexpr NSPoint to_native<NSPoint, point>(point p)
    {
        return { p.x, p.y };
    }
#endif // XAML_UI_WINDOWS

    template <typename TChar>
    inline std::tuple<double, double> __stot2d(std::basic_string_view<TChar> str)
    {
        std::size_t index = str.find_first_of(__delimeter<TChar>);
        if (index == std::basic_string_view<TChar>::npos)
        {
            double d = stof<double>(str);
            return std::make_tuple(d, d);
        }
        std::size_t index2 = str.find_first_not_of(__delimeter<TChar>, index);
        return std::make_tuple(stof<double>(str.substr(0, index)), stof<double>(str.substr(index2)));
    }

    inline std::tuple<double, double> stot2d(std::string_view str) { return __stot2d<char>(str); }
    inline std::tuple<double, double> stot2d(std::wstring_view str) { return __stot2d<wchar_t>(str); }

    template <typename T>
    struct __can_stot2d : std::false_type
    {
    };

    template <>
    struct __can_stot2d<size> : std::true_type
    {
    };

    template <>
    struct __can_stot2d<point> : std::true_type
    {
    };

    template <>
    struct __can_stot2d<std::tuple<double, double>> : std::true_type
    {
    };

    template <>
    struct __can_stot2d<std::pair<double, double>> : std::true_type
    {
    };

    template <>
    struct __can_stot2d<std::array<double, 2>> : std::true_type
    {
    };

    template <typename T>
    constexpr bool __can_stot2d_v = __can_stot2d<T>::value;

    template <typename T>
    struct value_converter_traits<T, std::enable_if_t<__can_stot2d_v<T>>> : __tuple___value_converter_traits_helper<T, std::tuple<double, double>, __stot2d<char>, __stot2d<wchar_t>>
    {
    };

    struct rectangle
    {
        double x, y;
        double width, height;
    };

    constexpr bool operator==(rectangle const& lhs, rectangle const& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.width == rhs.width && lhs.height == rhs.height; }
    constexpr bool operator!=(rectangle const& lhs, rectangle const& rhs) { return !(lhs == rhs); }

    constexpr rectangle operator+(point lhs, size rhs) { return { lhs.x, lhs.y, rhs.width, rhs.height }; }

    constexpr rectangle operator*(rectangle const& lhs, double rhs) { return { lhs.x * rhs, lhs.y * rhs, lhs.width * rhs, lhs.height * rhs }; }
    constexpr rectangle operator*(double lhs, rectangle const& rhs) { return rhs * lhs; }
    constexpr rectangle operator/(rectangle const& lhs, double rhs) { return lhs * (1 / rhs); }

#ifdef XAML_UI_WINDOWS
    constexpr rectangle from_native(RECT r)
    {
        return { (double)r.left, (double)r.top, (double)r.right - r.left, (double)r.bottom - r.top };
    }
    inline rectangle from_native(Gdiplus::RectF r)
    {
        return { (double)r.X, (double)r.Y, (double)r.Width, (double)r.Height };
    }
    template <>
    constexpr RECT to_native<RECT, rectangle>(rectangle r)
    {
        return { (LONG)r.x, (LONG)r.y, (LONG)(r.x + r.width), (LONG)(r.y + r.height) };
    }
    template <>
    inline Gdiplus::RectF to_native<Gdiplus::RectF, rectangle>(rectangle r)
    {
        return { (float)r.x, (float)r.y, (float)r.width, (float)r.height };
    }
#elif defined(XAML_UI_GTK3)
    constexpr rectangle from_native(GdkRectangle const& r)
    {
        return { (double)r.x, (double)r.y, (double)r.width, (double)r.height };
    }
    template <>
    constexpr GdkRectangle to_native<GdkRectangle, rectangle>(rectangle r)
    {
        return { (int)r.x, (int)r.y, (int)r.width, (int)r.height };
    }
#elif defined(XAML_UI_COCOA) && defined(__OBJC__)
    constexpr rectangle from_native(NSRect const& r)
    {
        return { r.origin.x, r.origin.y, r.size.width, r.size.height };
    }
    template <>
    constexpr NSRect to_native<NSRect, rectangle>(rectangle r)
    {
        return { { r.x, r.y }, { r.width, r.height } };
    }
#endif // XAML_UI_WINDOWS

    struct margin
    {
        double left;
        double top;
        double right;
        double bottom;
    };

    constexpr bool operator==(margin const& lhs, margin const& rhs) { return lhs.left == rhs.left && lhs.top == rhs.top && lhs.right == rhs.right && lhs.bottom == rhs.bottom; }
    constexpr bool operator!=(margin const& lhs, margin const& rhs) { return !(lhs == rhs); }

    constexpr rectangle operator+(rectangle const& lhs, margin const& rhs) { return { lhs.x - rhs.left, lhs.y - rhs.top, lhs.width + rhs.left + rhs.right, lhs.height + rhs.top + rhs.bottom }; }
    constexpr rectangle operator-(rectangle const& lhs, margin const& rhs) { return { lhs.x + rhs.left, lhs.y + rhs.top, lhs.width - rhs.left - rhs.right, lhs.height - rhs.top - rhs.bottom }; }

    constexpr margin operator*(margin const& lhs, double rhs) { return { lhs.left * rhs, lhs.top * rhs, lhs.right * rhs, lhs.bottom * rhs }; }
    constexpr margin operator*(double lhs, margin const& rhs) { return rhs * lhs; }
    constexpr margin operator/(margin const& lhs, double rhs) { return lhs * (1 / rhs); }

    template <typename TChar>
    inline std::tuple<double, double, double, double> __stot4d(std::basic_string_view<TChar> str)
    {
        std::size_t len1 = str.find_first_of(__delimeter<TChar>);
        if (len1 == std::basic_string_view<TChar>::npos)
        {
            double d = stof<double>(str);
            return std::make_tuple(d, d, d, d);
        }
        std::size_t index2 = str.find_first_not_of(__delimeter<TChar>, len1);
        std::size_t len2 = str.find_first_of(__delimeter<TChar>, index2);
        if (len2 == std::basic_string_view<TChar>::npos)
        {
            double d1 = stof<double>(str.substr(0, len1));
            double d2 = stof<double>(str.substr(index2));
            return std::make_tuple(d1, d2, d1, d2);
        }
        std::size_t index3 = str.find_first_not_of(__delimeter<TChar>, len2);
        std::size_t len3 = str.find_first_of(__delimeter<TChar>, index3);
        std::size_t index4 = str.find_first_not_of(__delimeter<TChar>, len3);
        return std::make_tuple(stof<double>(str.substr(0, len1)), stof<double>(str.substr(index2, len2 - index2)), stof<double>(str.substr(index3, len3 - index3)), stof<double>(str.substr(index4)));
    }

    inline std::tuple<double, double, double, double> stot4d(std::string_view str) { return __stot4d<char>(str); }
    inline std::tuple<double, double, double, double> stot4d(std::wstring_view str) { return __stot4d<wchar_t>(str); }

    template <typename T>
    struct __can_stot4d : std::false_type
    {
    };

    template <>
    struct __can_stot4d<rectangle> : std::true_type
    {
    };

    template <>
    struct __can_stot4d<margin> : std::true_type
    {
    };

    template <>
    struct __can_stot4d<std::tuple<double, double, double, double>> : std::true_type
    {
    };

    template <>
    struct __can_stot4d<std::array<double, 4>> : std::true_type
    {
    };

    template <typename T>
    constexpr bool __can_stot4d_v = __can_stot4d<T>::value;

    template <typename T>
    struct value_converter_traits<T, std::enable_if_t<__can_stot4d_v<T>>> : __tuple___value_converter_traits_helper<T, std::tuple<double, double, double, double>, __stot4d<char>, __stot4d<wchar_t>>
    {
    };

    struct alignas(1) color
    {
        std::uint8_t a, r, g, b;
        constexpr operator std::int32_t() const noexcept { return ((std::int32_t)a << 24) + ((std::int32_t)r << 16) + ((std::int32_t)g << 8) + (std::int32_t)b; }
        static constexpr color from_argb(std::int32_t v) noexcept { return { (std::uint8_t)((v >> 24) & 0xFF), (std::uint8_t)((v >> 16) & 0xFF), (std::uint8_t)((v >> 8) & 0xFF), (std::uint8_t)(v & 0xFF) }; }
    };

    constexpr bool operator==(color lhs, color rhs) { return (std::int32_t)lhs == (std::int32_t)rhs; }
    constexpr bool operator!=(color lhs, color rhs) { return !(lhs == rhs); }

    namespace colors
    {
        inline constexpr color alice_blue{ 0xFF, 0xF0, 0xF8, 0xFF };
        inline constexpr color antique_white{ 0xFF, 0xFA, 0xEB, 0xD7 };
        inline constexpr color aqua{ 0xFF, 0x00, 0xFF, 0xFF };
        inline constexpr color aquamarine{ 0xFF, 0x7F, 0xFF, 0xD4 };
        inline constexpr color azure{ 0xFF, 0xF0, 0xFF, 0xFF };
        inline constexpr color beige{ 0xFF, 0xF5, 0xF5, 0xDC };
        inline constexpr color bisque{ 0xFF, 0xFF, 0xE4, 0xC4 };
        inline constexpr color black{ 0xFF, 0x00, 0x00, 0x00 };
        inline constexpr color blanched_almond{ 0xFF, 0xFF, 0xEB, 0xCD };
        inline constexpr color blue{ 0xFF, 0x00, 0x00, 0xFF };
        inline constexpr color blue_violet{ 0xFF, 0x8A, 0x2B, 0xE2 };
        inline constexpr color brown{ 0xFF, 0xA5, 0x2A, 0x2A };
        inline constexpr color burly_wood{ 0xFF, 0xDE, 0xB8, 0x87 };
        inline constexpr color cadet_blue{ 0xFF, 0x5F, 0x9E, 0xA0 };
        inline constexpr color chartreuse{ 0xFF, 0x7F, 0xFF, 0x00 };
        inline constexpr color chocolate{ 0xFF, 0xD2, 0x69, 0x1E };
        inline constexpr color coral{ 0xFF, 0xFF, 0x7F, 0x50 };
        inline constexpr color cornflower_blue{ 0xFF, 0x64, 0x95, 0xED };
        inline constexpr color cornsilk{ 0xFF, 0xFF, 0xF8, 0xDC };
        inline constexpr color crimson{ 0xFF, 0xDC, 0x14, 0x3C };
        inline constexpr color cyan{ 0xFF, 0x00, 0xFF, 0xFF };
        inline constexpr color dark_blue{ 0xFF, 0x00, 0x00, 0x8B };
        inline constexpr color dark_cyan{ 0xFF, 0x00, 0x8B, 0x8B };
        inline constexpr color dark_goldenrod{ 0xFF, 0xB8, 0x86, 0x0B };
        inline constexpr color dark_gray{ 0xFF, 0xA9, 0xA9, 0xA9 };
        inline constexpr color dark_green{ 0xFF, 0x00, 0x64, 0x00 };
        inline constexpr color dark_khaki{ 0xFF, 0xBD, 0xB7, 0x6B };
        inline constexpr color dark_magenta{ 0xFF, 0x8B, 0x00, 0x8B };
        inline constexpr color dark_olive_green{ 0xFF, 0x55, 0x6B, 0x2F };
        inline constexpr color dark_orange{ 0xFF, 0xFF, 0x8C, 0x00 };
        inline constexpr color dark_orchid{ 0xFF, 0x99, 0x32, 0xCC };
        inline constexpr color dark_red{ 0xFF, 0x8B, 0x00, 0x00 };
        inline constexpr color dark_salmon{ 0xFF, 0xE9, 0x96, 0x7A };
        inline constexpr color dark_sea_green{ 0xFF, 0x8F, 0xBC, 0x8B };
        inline constexpr color dark_slate_blue{ 0xFF, 0x48, 0x3D, 0x8B };
        inline constexpr color dark_slate_gray{ 0xFF, 0x2F, 0x4F, 0x4F };
        inline constexpr color dark_turquoise{ 0xFF, 0x00, 0xCE, 0xD1 };
        inline constexpr color dark_violet{ 0xFF, 0x94, 0x00, 0xD3 };
        inline constexpr color deep_pink{ 0xFF, 0xFF, 0x14, 0x93 };
        inline constexpr color deep_sky_blue{ 0xFF, 0x00, 0xBF, 0xFF };
        inline constexpr color dim_gray{ 0xFF, 0x69, 0x69, 0x69 };
        inline constexpr color dodger_blue{ 0xFF, 0x1E, 0x90, 0xFF };
        inline constexpr color firebrick{ 0xFF, 0xB2, 0x22, 0x22 };
        inline constexpr color floral_white{ 0xFF, 0xFF, 0xFA, 0xF0 };
        inline constexpr color forest_green{ 0xFF, 0x22, 0x8B, 0x22 };
        inline constexpr color fuchsia{ 0xFF, 0xFF, 0x00, 0xFF };
        inline constexpr color gainsboro{ 0xFF, 0xDC, 0xDC, 0xDC };
        inline constexpr color ghost_white{ 0xFF, 0xF8, 0xF8, 0xFF };
        inline constexpr color gold{ 0xFF, 0xFF, 0xD7, 0x00 };
        inline constexpr color goldenrod{ 0xFF, 0xDA, 0xA5, 0x20 };
        inline constexpr color gray{ 0xFF, 0x80, 0x80, 0x80 };
        inline constexpr color green{ 0xFF, 0x00, 0x80, 0x00 };
        inline constexpr color green_yellow{ 0xFF, 0xAD, 0xFF, 0x2F };
        inline constexpr color honeydew{ 0xFF, 0xF0, 0xFF, 0xF0 };
        inline constexpr color hot_pink{ 0xFF, 0xFF, 0x69, 0xB4 };
        inline constexpr color indian_red{ 0xFF, 0xCD, 0x5C, 0x5C };
        inline constexpr color indigo{ 0xFF, 0x4B, 0x00, 0x82 };
        inline constexpr color ivory{ 0xFF, 0xFF, 0xFF, 0xF0 };
        inline constexpr color khaki{ 0xFF, 0xF0, 0xE6, 0x8C };
        inline constexpr color lavender{ 0xFF, 0xE6, 0xE6, 0xFA };
        inline constexpr color lavender_blush{ 0xFF, 0xFF, 0xF0, 0xF5 };
        inline constexpr color lawn_green{ 0xFF, 0x7C, 0xFC, 0x00 };
        inline constexpr color lemon_chiffon{ 0xFF, 0xFF, 0xFA, 0xCD };
        inline constexpr color light_blue{ 0xFF, 0xAD, 0xD8, 0xE6 };
        inline constexpr color light_coral{ 0xFF, 0xF0, 0x80, 0x80 };
        inline constexpr color light_cyan{ 0xFF, 0xE0, 0xFF, 0xFF };
        inline constexpr color light_goldenrod_yellow{ 0xFF, 0xFA, 0xFA, 0xD2 };
        inline constexpr color light_gray{ 0xFF, 0xD3, 0xD3, 0xD3 };
        inline constexpr color light_green{ 0xFF, 0x90, 0xEE, 0x90 };
        inline constexpr color light_pink{ 0xFF, 0xFF, 0xB6, 0xC1 };
        inline constexpr color light_salmon{ 0xFF, 0xFF, 0xA0, 0x7A };
        inline constexpr color light_sea_green{ 0xFF, 0x20, 0xB2, 0xAA };
        inline constexpr color light_sky_blue{ 0xFF, 0x87, 0xCE, 0xFA };
        inline constexpr color light_slate_gray{ 0xFF, 0x77, 0x88, 0x99 };
        inline constexpr color light_steel_blue{ 0xFF, 0xB0, 0xC4, 0xDE };
        inline constexpr color light_yellow{ 0xFF, 0xFF, 0xFF, 0xE0 };
        inline constexpr color lime{ 0xFF, 0x00, 0xFF, 0x00 };
        inline constexpr color lime_green{ 0xFF, 0x32, 0xCD, 0x32 };
        inline constexpr color linen{ 0xFF, 0xFA, 0xF0, 0xE6 };
        inline constexpr color magenta{ 0xFF, 0xFF, 0x00, 0xFF };
        inline constexpr color maroon{ 0xFF, 0x80, 0x00, 0x00 };
        inline constexpr color medium_aquamarine{ 0xFF, 0x66, 0xCD, 0xAA };
        inline constexpr color medium_blue{ 0xFF, 0x00, 0x00, 0xCD };
        inline constexpr color medium_orchid{ 0xFF, 0xBA, 0x55, 0xD3 };
        inline constexpr color medium_purple{ 0xFF, 0x93, 0x70, 0xDB };
        inline constexpr color medium_sea_green{ 0xFF, 0x3C, 0xB3, 0x71 };
        inline constexpr color medium_slate_blue{ 0xFF, 0x7B, 0x68, 0xEE };
        inline constexpr color medium_spring_green{ 0xFF, 0x00, 0xFA, 0x9A };
        inline constexpr color medium_turquoise{ 0xFF, 0x48, 0xD1, 0xCC };
        inline constexpr color medium_violet_red{ 0xFF, 0xC7, 0x15, 0x85 };
        inline constexpr color midnight_blue{ 0xFF, 0x19, 0x19, 0x70 };
        inline constexpr color mint_cream{ 0xFF, 0xF5, 0xFF, 0xFA };
        inline constexpr color misty_rose{ 0xFF, 0xFF, 0xE4, 0xE1 };
        inline constexpr color moccasin{ 0xFF, 0xFF, 0xE4, 0xB5 };
        inline constexpr color navajo_white{ 0xFF, 0xFF, 0xDE, 0xAD };
        inline constexpr color navy{ 0xFF, 0x00, 0x00, 0x80 };
        inline constexpr color old_lace{ 0xFF, 0xFD, 0xF5, 0xE6 };
        inline constexpr color olive{ 0xFF, 0x80, 0x80, 0x00 };
        inline constexpr color olive_drab{ 0xFF, 0x6B, 0x8E, 0x23 };
        inline constexpr color orange{ 0xFF, 0xFF, 0xA5, 0x00 };
        inline constexpr color orange_red{ 0xFF, 0xFF, 0x45, 0x00 };
        inline constexpr color orchid{ 0xFF, 0xDA, 0x70, 0xD6 };
        inline constexpr color pale_goldenrod{ 0xFF, 0xEE, 0xE8, 0xAA };
        inline constexpr color pale_green{ 0xFF, 0x98, 0xFB, 0x98 };
        inline constexpr color pale_turquoise{ 0xFF, 0xAF, 0xEE, 0xEE };
        inline constexpr color pale_violet_red{ 0xFF, 0xDB, 0x70, 0x93 };
        inline constexpr color papaya_whip{ 0xFF, 0xFF, 0xEF, 0xD5 };
        inline constexpr color peach_puff{ 0xFF, 0xFF, 0xDA, 0xB9 };
        inline constexpr color peru{ 0xFF, 0xCD, 0x85, 0x3F };
        inline constexpr color pink{ 0xFF, 0xFF, 0xC0, 0xCB };
        inline constexpr color plum{ 0xFF, 0xDD, 0xA0, 0xDD };
        inline constexpr color powder_blue{ 0xFF, 0xB0, 0xE0, 0xE6 };
        inline constexpr color purple{ 0xFF, 0x80, 0x00, 0x80 };
        inline constexpr color red{ 0xFF, 0xFF, 0x00, 0x00 };
        inline constexpr color rosy_brown{ 0xFF, 0xBC, 0x8F, 0x8F };
        inline constexpr color royal_blue{ 0xFF, 0x41, 0x69, 0xE1 };
        inline constexpr color saddle_brown{ 0xFF, 0x8B, 0x45, 0x13 };
        inline constexpr color salmon{ 0xFF, 0xFA, 0x80, 0x72 };
        inline constexpr color sandy_brown{ 0xFF, 0xF4, 0xA4, 0x60 };
        inline constexpr color sea_green{ 0xFF, 0x2E, 0x8B, 0x57 };
        inline constexpr color sea_shell{ 0xFF, 0xFF, 0xF5, 0xEE };
        inline constexpr color sienna{ 0xFF, 0xA0, 0x52, 0x2D };
        inline constexpr color silver{ 0xFF, 0xC0, 0xC0, 0xC0 };
        inline constexpr color sky_blue{ 0xFF, 0x87, 0xCE, 0xEB };
        inline constexpr color slate_blue{ 0xFF, 0x6A, 0x5A, 0xCD };
        inline constexpr color slate_gray{ 0xFF, 0x70, 0x80, 0x90 };
        inline constexpr color snow{ 0xFF, 0xFF, 0xFA, 0xFA };
        inline constexpr color spring_green{ 0xFF, 0x00, 0xFF, 0x7F };
        inline constexpr color steel_blue{ 0xFF, 0x46, 0x82, 0xB4 };
        inline constexpr color tan{ 0xFF, 0xD2, 0xB4, 0x8C };
        inline constexpr color teal{ 0xFF, 0x00, 0x80, 0x80 };
        inline constexpr color thistle{ 0xFF, 0xD8, 0xBF, 0xD8 };
        inline constexpr color tomato{ 0xFF, 0xFF, 0x63, 0x47 };
        inline constexpr color transparent{ 0x00, 0xFF, 0xFF, 0xFF };
        inline constexpr color turquoise{ 0xFF, 0x40, 0xE0, 0xD0 };
        inline constexpr color violet{ 0xFF, 0xEE, 0x82, 0xEE };
        inline constexpr color wheat{ 0xFF, 0xF5, 0xDE, 0xB3 };
        inline constexpr color white{ 0xFF, 0xFF, 0xFF, 0xFF };
        inline constexpr color white_smoke{ 0xFF, 0xF5, 0xF5, 0xF5 };
        inline constexpr color yellow{ 0xFF, 0xFF, 0xFF, 0x00 };
        inline constexpr color yellow_green{ 0xFF, 0x9A, 0xCD, 0x32 };
    } // namespace colors
} // namespace xaml
#endif // !XAML_UI_DRAWING_HPP
