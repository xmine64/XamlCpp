#ifdef XAML_UI_CANVAS_DIRECT2D

#include <tuple>
#include <win/canvas_d2d.hpp>
#include <xaml/ui/native_control.hpp>
#include <xaml/ui/native_window.hpp>

using namespace std;

namespace xaml
{
    drawing_context_d2d::~drawing_context_d2d() {}

    static inline D2D1::ColorF get_COLOR(color c)
    {
        return (uint32_t)c;
    }

    static wil::com_ptr<ID2D1Brush> get_Brush(ID2D1RenderTarget* target, color c)
    {
        wil::com_ptr<ID2D1SolidColorBrush> brush;
        target->CreateSolidColorBrush(get_COLOR(c), &brush);
        return brush;
    }

    static D2D1_ELLIPSE get_ELLIPSE(rectangle const& region)
    {
        return D2D1::Ellipse(D2D1::Point2F((FLOAT)(region.x + region.width / 2), (FLOAT)(region.y + region.height / 2)), (FLOAT)(region.width / 2), (FLOAT)(region.height / 2));
    }

#define CHECK_SIZE(r) \
    if ((r).width < 1 || (r).height < 1) return

    static constexpr tuple<size, point, point, point> get_arc(rectangle const& region, double start_angle, double end_angle)
    {
        size radius = { region.width / 2, region.height / 2 };
        point centerp = { region.x + radius.width, region.y + radius.height };
        point startp = centerp + point{ radius.width * cos(start_angle), radius.height * sin(start_angle) };
        point endp = centerp + point{ radius.width * cos(end_angle), radius.height * sin(end_angle) };
        return make_tuple(radius, centerp, startp, endp);
    }

    void drawing_context_d2d::draw_arc(drawing_pen const& pen, rectangle const& region, double start_angle, double end_angle)
    {
        CHECK_SIZE(region);
        wil::com_ptr<ID2D1PathGeometry> geo;
        THROW_IF_FAILED(d2d->CreatePathGeometry(&geo));
        wil::com_ptr<ID2D1GeometrySink> sink;
        THROW_IF_FAILED(geo->Open(&sink));
        auto [radius, centerp, startp, endp] = get_arc(region, start_angle, end_angle);
        sink->BeginFigure(to_native<D2D1_POINT_2F>(startp), D2D1_FIGURE_BEGIN_HOLLOW);
        sink->AddArc(D2D1::ArcSegment(to_native<D2D1_POINT_2F>(endp), to_native<D2D1_SIZE_F>(radius), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, ((end_angle - start_angle) > M_PI) ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL));
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        THROW_IF_FAILED(sink->Close());
        auto b = get_Brush(target.get(), pen.stroke);
        target->DrawGeometry(geo.get(), b.get(), (FLOAT)pen.width);
    }

    void drawing_context_d2d::fill_pie(drawing_brush const& brush, rectangle const& region, double start_angle, double end_angle)
    {
        CHECK_SIZE(region);
        wil::com_ptr<ID2D1PathGeometry> geo;
        THROW_IF_FAILED(d2d->CreatePathGeometry(&geo));
        wil::com_ptr<ID2D1GeometrySink> sink;
        THROW_IF_FAILED(geo->Open(&sink));
        auto [radius, centerp, startp, endp] = get_arc(region, start_angle, end_angle);
        sink->BeginFigure(to_native<D2D1_POINT_2F>(centerp), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(to_native<D2D1_POINT_2F>(startp));
        sink->AddArc(D2D1::ArcSegment(to_native<D2D1_POINT_2F>(endp), to_native<D2D1_SIZE_F>(radius), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, ((end_angle - start_angle) > M_PI) ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        THROW_IF_FAILED(sink->Close());
        auto b = get_Brush(target.get(), brush.fill);
        target->FillGeometry(geo.get(), b.get());
    }

    void drawing_context_d2d::draw_ellipse(drawing_pen const& pen, rectangle const& region)
    {
        CHECK_SIZE(region);
        auto e = get_ELLIPSE(region);
        auto b = get_Brush(target.get(), pen.stroke);
        target->DrawEllipse(e, b.get(), (FLOAT)pen.width);
    }

    void drawing_context_d2d::fill_ellipse(drawing_brush const& brush, rectangle const& region)
    {
        CHECK_SIZE(region);
        auto e = get_ELLIPSE(region);
        auto b = get_Brush(target.get(), brush.fill);
        target->FillEllipse(e, b.get());
    }

    void drawing_context_d2d::draw_line(drawing_pen const& pen, point startp, point endp)
    {
        auto b = get_Brush(target.get(), pen.stroke);
        target->DrawLine(to_native<D2D1_POINT_2F>(startp), to_native<D2D1_POINT_2F>(endp), b.get(), (FLOAT)pen.width);
    }

    void drawing_context_d2d::draw_rect(drawing_pen const& pen, rectangle const& rect)
    {
        CHECK_SIZE(rect);
        auto r = to_native<D2D1_RECT_F>(rect);
        auto b = get_Brush(target.get(), pen.stroke);
        target->DrawRectangle(r, b.get(), (FLOAT)pen.width);
    }

    void drawing_context_d2d::fill_rect(drawing_brush const& brush, rectangle const& rect)
    {
        CHECK_SIZE(rect);
        auto r = to_native<D2D1_RECT_F>(rect);
        auto b = get_Brush(target.get(), brush.fill);
        target->FillRectangle(r, b.get());
    }

    void drawing_context_d2d::draw_round_rect(drawing_pen const& pen, rectangle const& rect, size round)
    {
        CHECK_SIZE(rect);
        auto r = to_native<D2D1_RECT_F>(rect);
        auto s = to_native<D2D1_SIZE_F>(round);
        auto b = get_Brush(target.get(), pen.stroke);
        target->DrawRoundedRectangle(D2D1::RoundedRect(r, s.width, s.height), b.get(), (FLOAT)pen.width);
    }

    void drawing_context_d2d::fill_round_rect(drawing_brush const& brush, rectangle const& rect, size round)
    {
        CHECK_SIZE(rect);
        auto r = to_native<D2D1_RECT_F>(rect);
        auto s = to_native<D2D1_SIZE_F>(round);
        auto b = get_Brush(target.get(), brush.fill);
        target->FillRoundedRectangle(D2D1::RoundedRect(r, s.width, s.height), b.get());
    }

    void drawing_context_d2d::draw_string(drawing_brush const& brush, drawing_font const& font, point p, string_view_t str)
    {
        auto b = get_Brush(target.get(), brush.fill);
        auto fsize = (FLOAT)font.size;
        if (fsize <= 0) return;
        wil::com_ptr<IDWriteTextFormat> format;
        THROW_IF_FAILED(dwrite->CreateTextFormat(
            font.font_family.c_str(), nullptr,
            font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, fsize, L"", &format));
        auto size = target->GetSize();
        auto region = to_native<D2D1_RECT_F, rectangle>({ p.x, p.y, p.x, p.y });
        region.right += size.width;
        region.bottom += size.height;
        target->DrawText(str.data(), (UINT32)str.length(), format.get(), region, b.get());
    }

    static ID2D1Factory* try_create_factory()
    {
        ID2D1Factory* factory = nullptr;
        __try
        {
            HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
            if (FAILED(hr)) factory = nullptr;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            factory = nullptr;
        }
        return factory;
    }

    canvas_d2d::canvas_d2d() {}

    canvas_d2d::~canvas_d2d() {}

    bool canvas_d2d::create(shared_ptr<window> wnd, rectangle const& real)
    {
        if (!d2d)
        {
            if (d2d = try_create_factory())
            {
                try
                {
                    auto prop = D2D1::RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        D2D1::PixelFormat(
                            DXGI_FORMAT_B8G8R8A8_UNORM,
                            D2D1_ALPHA_MODE_PREMULTIPLIED),
                        0,
                        0,
                        D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
                        D2D1_FEATURE_LEVEL_DEFAULT);
                    THROW_IF_FAILED(d2d->CreateDCRenderTarget(&prop, &target));
                    THROW_IF_FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), dwrite.put_unknown()));
                    return true;
                }
                catch (wil::ResultException const&)
                {
                    return false;
                }
            }
        }
        return false;
    }

    void canvas_d2d::begin_paint(shared_ptr<window> wnd, rectangle const& real, function<void(drawing_context&)> paint_func)
    {
        double dpi = wnd->get_dpi();
        rectangle region = real * dpi / 96.0;
        CHECK_SIZE(region);
        RECT rc_region = to_native<RECT>(region);
        THROW_IF_FAILED(target->BindDC(wnd->get_window()->store_dc.get(), &rc_region));
        target->BeginDraw();
        target->SetDpi((FLOAT)dpi, (FLOAT)dpi);
        target->Clear(D2D1::ColorF(D2D1::ColorF::White));
        drawing_context_d2d ctx{};
        ctx.d2d = d2d.copy<ID2D1Factory>();
        ctx.target = target.query<ID2D1RenderTarget>();
        ctx.dwrite = dwrite.copy<IDWriteFactory>();
        drawing_context dc{ &ctx };
        paint_func(dc);
        THROW_IF_FAILED(target->EndDraw());
    }
} // namespace xaml

#endif // XAML_UI_CANVAS_DIRECT2D
