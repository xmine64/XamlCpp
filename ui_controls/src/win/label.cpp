#include <wil/result_macros.h>
#include <windowsx.h>
#include <xaml/ui/controls/label.hpp>
#include <xaml/ui/native_control.hpp>

#include <CommCtrl.h>

using namespace std;

namespace xaml
{
    void label::__draw(rectangle const& region)
    {
        if (auto sparent = get_parent().lock())
        {
            if (!get_handle())
            {
                window_create_params params = {};
                params.class_name = WC_STATIC;
                params.style = WS_CHILD | WS_VISIBLE | SS_LEFT;
                params.x = 0;
                params.y = 0;
                params.width = 100;
                params.height = 50;
                params.parent = sparent.get();
                this->__create(params);
                draw_text();
                draw_alignment();
                SetParent(get_handle()->handle, sparent->get_handle()->handle);
            }
            __set_rect(region);
        }
    }

    void label::draw_text()
    {
        THROW_IF_WIN32_BOOL_FALSE(Static_SetText(get_handle()->handle, m_text.c_str()));
    }

    void label::draw_alignment()
    {
        LONG_PTR style = WS_CHILD | WS_VISIBLE;
        switch (m_text_halignment)
        {
        case halignment_t::center:
            style |= SS_CENTER;
            break;
        case halignment_t::right:
            style |= SS_RIGHT;
            break;
        default:
            style |= SS_LEFT;
            break;
        }
        SetWindowLongPtr(get_handle()->handle, GWL_STYLE, style);
    }

    void label::__size_to_fit()
    {
        __set_size_noevent(__measure_text_size(m_text, {}));
    }
} // namespace xaml
