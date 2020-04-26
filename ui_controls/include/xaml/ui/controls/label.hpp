#ifndef XAML_UI_LABEL_HPP
#define XAML_UI_LABEL_HPP

#include <atomic>
#include <xaml/meta/default_property.hpp>
#include <xaml/ui/control.hpp>

namespace xaml
{
    class label;

    template <>
    struct type_guid<label>
    {
        static constexpr guid value{ 0xa859441c, 0x825c, 0x4682, { 0x8e, 0x04, 0xe2, 0x4d, 0x62, 0x59, 0xe0, 0x87 } };
    };

    class label : public control
    {
    public:
        META_CLASS_IMPL(control)

    public:
        XAML_UI_CONTROLS_API label();
        XAML_UI_CONTROLS_API ~label() override;

#ifdef XAML_UI_WINDOWS
    public:
        XAML_UI_CONTROLS_API void __int32_to_fit() override;
#endif // XAML_UI_WINDOWS

    public:
        XAML_UI_CONTROLS_API void __draw(rectangle const& region) override;

    protected:
        XAML_UI_CONTROLS_API virtual void draw_text();
        XAML_UI_CONTROLS_API virtual void draw_alignment();

        PROP(text_halignment, halignment_t)

        EVENT(text_changed, std::shared_ptr<label>, string_view_t)
        PROP_STRING_EVENT(text)

    public:
#define ADD_LABEL_MEMBERS()    \
    ADD_CONTROL_MEMBERS();     \
    ADD_PROP_EVENT(text);      \
    ADD_PROP(text_halignment); \
    ADD_DEF_PROP(text)

        REGISTER_CLASS_DECL(xaml, label, "xaml/ui/controls")
        {
            ADD_CTOR();
            ADD_LABEL_MEMBERS();
        }
        REGISTER_CLASS_END()
    };
} // namespace xaml

#endif // !XAML_UI_LABEL_HPP
