#include <shared/atomic_guard.hpp>
#include <win/webview_ie.hpp>
#include <xaml/ui/controls/native_webview.hpp>
#include <xaml/ui/controls/webview.hpp>
#include <xaml/ui/native_control.hpp>

#ifdef XAML_UI_WEBVIEW_EDGE
#include <win/webview_edge.hpp>
#endif // XAML_UI_WEBVIEW_EDGE

#ifdef XAML_UI_WEBVIEW_WEBVIEW2
#include <win/webview_edge2.hpp>
#endif // XAML_UI_WEBVIEW_WEBVIEW2

using namespace std;

namespace xaml
{
    void webview::create_edge2(rectangle const& real)
    {
#ifdef XAML_UI_WEBVIEW_WEBVIEW2
        m_webview = make_shared<webview_edge2>();
        m_webview->create_async((intptr_t)get_handle()->handle, real, [this, real]() {
            if (!*m_webview)
            {
                create_edge(real);
            }
            else
            {
                draw_create();
            }
        });

#else
        create_edge(real);
#endif // XAML_UI_WEBVIEW_WEBVIEW2
    }

    void webview::create_edge(rectangle const& real)
    {
#ifdef XAML_UI_WEBVIEW_EDGE
        m_webview = make_shared<webview_edge>();
        m_webview->create_async((intptr_t)get_handle()->handle, real, [this, real]() {
            if (!*m_webview)
            {
                create_ie(real);
            }
            else
            {
                draw_create();
            }
        });

#else
        create_ie(real);
#endif // XAML_UI_WEBVIEW_EDGE
    }

    void webview::create_ie(rectangle const& real)
    {
        m_webview = make_shared<webview_ie>();
        m_webview->create_async((intptr_t)get_handle()->handle, real, [this]() {
            if (!*m_webview)
            {
                m_webview = nullptr;
            }
            else
            {
                draw_create();
            }
        });
    }

    void webview::__draw(rectangle const& region)
    {
        if (auto sparent = get_parent().lock())
        {
            set_handle(sparent->get_handle());
            rectangle real = region - get_margin();
            UINT udpi = GetDpiForWindow(get_handle()->handle);
            rectangle real_real = real * udpi / 96.0;
            if (!get_webview())
            {
                create_edge2(real_real);
            }
            __set_size_noevent({ real.width, real.height });
            if (get_webview() && *get_webview()) m_webview->set_rect(real_real);
        }
    }

    void webview::draw_create()
    {
        m_webview->set_navigated([this](string_view_t uri) {
            atomic_guard guard(m_navigating);
            if (!guard.exchange(true))
            {
                set_uri(uri);
            }
        });
        m_webview->set_resource_requested([this](resource_requested_args& args) { m_resource_requested(*this, args); });
        draw_uri();
        __parent_redraw();
    }

    void webview::draw_size()
    {
        if (get_webview() && *get_webview())
        {
            m_webview->set_size(__get_real_size());
        }
    }

    void webview::draw_uri()
    {
        if (get_webview() && *get_webview())
        {
            m_webview->navigate(get_uri());
        }
    }
} // namespace xaml
