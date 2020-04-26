#include <cmath>
#include <shared/uniform_grid.hpp>
#include <xaml/ui/controls/uniform_grid.h>

using namespace std;

//namespace xaml
//{
//    uniform_grid::uniform_grid() : layout_base() {}
//
//    uniform_grid::~uniform_grid() {}
//
//    void uniform_grid::__draw_impl(rectangle const& region, function<void(shared_ptr<control>, rectangle const&)> func)
//    {
//        if (!get_children().empty())
//        {
//            layout_base::__draw_impl(region, func);
//            rectangle real = region - get_margin();
//            int32_t cs = get_columns();
//            int32_t rs = get_rows();
//            int32_t n = get_children().size();
//            if (cs > 0 && rs > 0)
//            {
//                if (get_orientation() == orientation::vertical)
//                {
//                    rs = (n + cs - 1) / cs;
//                }
//                else
//                {
//                    cs = (n + rs - 1) / rs;
//                }
//            }
//            else
//            {
//                if (cs > 0)
//                {
//                    rs = (n + cs - 1) / cs;
//                }
//                else if (rs > 0)
//                {
//                    cs = (n + rs - 1) / rs;
//                }
//                else
//                {
//                    if (get_orientation() == orientation::vertical)
//                    {
//                        cs = (int32_t)sqrt(n);
//                        rs = (n + cs - 1) / cs;
//                    }
//                    else
//                    {
//                        rs = (int32_t)sqrt(n);
//                        cs = (n + rs - 1) / rs;
//                    }
//                }
//            }
//            double w = real.width / cs;
//            double h = real.height / rs;
//            int32_t x = 0, y = 0;
//            if (get_orientation() == orientation::vertical)
//            {
//                for (auto& c : get_children())
//                {
//                    rectangle subrect = { real.x + x * w, real.y + y * h, w, h };
//                    c->__draw(subrect);
//                    if (func) func(c, subrect);
//                    y++;
//                    if (y >= rs)
//                    {
//                        y = 0;
//                        x++;
//                    }
//                }
//            }
//            else
//            {
//                for (auto& c : get_children())
//                {
//                    rectangle subrect = { real.x + x * w, real.y + y * h, w, h };
//                    c->__draw(subrect);
//                    if (func) func(c, subrect);
//                    x++;
//                    if (x >= cs)
//                    {
//                        x = 0;
//                        y++;
//                    }
//                }
//            }
//        }
//    }
//
//    void uniform_grid::__size_to_fit()
//    {
//        int32_t cs, rs;
//        int32_t n = get_children().size();
//        if (get_orientation() == orientation::vertical)
//        {
//            cs = (int32_t)sqrt(n);
//            rs = (n + cs - 1) / cs;
//        }
//        else
//        {
//            rs = (int32_t)sqrt(n);
//            cs = (n + rs - 1) / rs;
//        }
//        double mw = 0, mh = 0;
//        for (auto& c : get_children())
//        {
//            if (!c->get_handle()) c->__draw(rectangle{ 0, 0, 0, 0 } + c->get_margin());
//            c->__size_to_fit();
//            auto csize = c->get_size();
//            auto cmargin = c->get_margin();
//            csize.width += cmargin.left + cmargin.right;
//            csize.height += cmargin.top + cmargin.bottom;
//            if (csize.width > mw) mw = csize.width;
//            if (csize.height > mh) mh = csize.height;
//        }
//        __set_size_noevent({ mw * cs, mh * rs });
//    }
//} // namespace xaml
