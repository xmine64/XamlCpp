#ifndef XAML_UI_TIMER_HPP
#define XAML_UI_TIMER_HPP

#include <atomic>
#include <chrono>
#include <memory>
#include <xaml/meta/meta_macro.hpp>
#include <xaml/ui/objc.hpp>

#ifdef XAML_UI_WINDOWS
#include <Windows.h>
#elif defined(XAML_UI_WINRT)
#include "winrt/Windows.UI.Xaml.h"
#elif defined(XAML_UI_GTK3)
#include <gtk/gtk.h>
#elif defined(XAML_UI_COCOA) && defined(__OBJC__)
#import <Cocoa/Cocoa.h>
#endif

namespace xaml
{
    class timer
    {
    private:
        std::atomic<bool> m_enabled{ false };

    public:
        bool is_enabled() const noexcept { return m_enabled; }

#ifdef XAML_UI_WINDOWS
    private:
        UINT_PTR m_id;

    public:
        constexpr UINT_PTR __get_id() const noexcept { return m_id; }

    protected:
        void __set_id(UINT_PTR value) { m_id = value; }

    private:
        XAML_UI_API static void CALLBACK on_tick(HWND hWnd, UINT Msg, UINT_PTR nIdEvent, DWORD uElapsed);
#endif // XAML_UI_WINDOWS

#ifdef XAML_UI_GTK3
    private:
        static gboolean on_timeout(gpointer data);
#endif // XAML_UI_GTK3

#if defined(XAML_UI_COCOA) || defined(XAML_UI_WINRT)
    public:
#ifdef XAML_UI_WINRT
        using __native_handle_type = winrt::Windows::UI::Xaml::DispatcherTimer;
#elif defined(XAML_UI_COCOA)
        using __native_handle_type = OBJC_OBJECT(NSTimer);
#endif // XAML_UI_WINRT

    private:
        __native_handle_type m_handle{ OBJC_NIL };

    public:
        inline __native_handle_type __get_handle() const noexcept { return m_handle; }

    protected:
        void __set_handle(__native_handle_type value) OBJC_BLOCK({ m_handle = value; });

#ifdef XAML_UI_COCOA
    public:
        void __on_tick();
#endif // XAML_UI_COCOA
#endif // XAML_UI_COCOA || XAML_UI_WINRT

    private:
        std::chrono::milliseconds m_interval;

    public:
        constexpr std::chrono::milliseconds get_interval() const noexcept { return m_interval; }
        void set_interval(std::chrono::milliseconds value)
        {
            bool enabled = m_enabled;
            stop();
            m_interval = value;
            if (enabled) start();
        }

    public:
        timer(std::chrono::milliseconds interval = std::chrono::milliseconds{ 1 }) : m_interval(interval) {}
        ~timer() { stop(); }

#ifdef XAML_UI_WINRT
    private:
        XAML_UI_API void init();
#endif // XAML_UI_WINRT

    public:
        XAML_UI_API void start();
        XAML_UI_API void stop();

        EVENT(tick, timer&)
    };
} // namespace xaml

#endif // !XAML_UI_TIMER_HPP
