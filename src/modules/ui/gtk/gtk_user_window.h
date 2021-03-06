/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#ifndef _GTK_USER_WINDOW_H_
#define _GTK_USER_WINDOW_H_

namespace ti
{
    class GtkUserWindow : public UserWindow
    {
    public:
        GtkUserWindow(AutoPtr<WindowConfig>, AutoUserWindow&);
        inline WebKitWebView* GetWebView() { return this->webView; }

        virtual ~GtkUserWindow();
        void CreateWidgets();
        void ShowWidgets();
        void SetupDecorations();
        void SetupTransparency();
        void SetupSizeLimits();
        void SetupSize();
        void SetupPosition();
        void SetupMenu();
        void SetupIcon();
        virtual void AppMenuChanged();
        virtual void AppIconChanged();
        void RemoveOldMenu();
        void OpenFileChooserDialog(TiMethodRef callback,
            bool multiple, std::string& title, std::string& path,
            std::string& defaultName, std::vector<std::string>& types,
            std::string& typesDescription);
        void OpenFolderChooserDialog(TiMethodRef callback,
            bool multiple, std::string& title, std::string& path,
            std::string& defaultName);
        void OpenSaveAsDialog(TiMethodRef callback,
            std::string& title, std::string& path,
            std::string& defaultName, std::vector<std::string>& types,
            std::string& typesDescription);

        void Hide();
        void Show();
        void Minimize();
        void Maximize();
        void Unminimize();
        void Unmaximize();
        bool IsMinimized();
        bool IsMaximized();
        void Focus();
        void Unfocus();
        bool IsUsingChrome();
        void SetUsingChrome(bool chrome);
        bool IsUsingScrollbars();
        bool IsFullscreen();
        void Open();
        bool Close();
        double GetX();
        void SetX(double x);
        double GetY();
        void SetY(double y);
        void MoveTo(double x, double y);

        double GetWidth();
        void SetWidth(double width) ;
        double GetMaxWidth();
        void SetMaxWidth(double width);
        double GetMinWidth();
        void SetMinWidth(double width);

        double GetHeight();
        void SetHeight(double height);
        double GetMaxHeight();
        void SetMaxHeight(double height);
        double GetMinHeight();
        void SetMinHeight(double height);

        void SetSize(double width, double height);
        Bounds GetBoundsImpl();
        void SetBoundsImpl(Bounds bounds);
        std::string GetTitle();
        void SetTitleImpl(const std::string& title);
        std::string GetURL();
        void SetURL(std::string& url);
        bool IsResizable();
        void SetResizableImpl(bool resizable);
        bool IsMaximizable();
        void SetMaximizable(bool maximizable);
        bool IsMinimizable();
        void SetMinimizable(bool minimizable);
        bool IsCloseable();
        void SetCloseable(bool closeable);
        bool IsVisible();
        double GetTransparency();
        void SetTransparency(double transparency);
        void SetFullscreen(bool fullscreen);
        bool IsTopMost();
        void SetTopMost(bool topmost);
        void SetMenu(AutoMenu);
        AutoMenu GetMenu();
        void SetContextMenu(AutoMenu);
        AutoMenu GetContextMenu();
        void SetIcon(std::string& iconPath);
        std::string& GetIcon();
        void SetInspectorWindow(GtkWidget* inspectorWindow);
        GtkWidget *GetInspectorWindow();
        virtual void ShowInspector(bool console);
        void SetContentsImpl(const std::string&, const std::string&);
        void SetPluginsEnabled(bool enabled);

        // These values contain the most-recently-set dimension
        // information for this UserWindow. GDK is asynchronous,
        // so if a user sets the value the and fetches it without
        // giving up control to the UI thread, returning one of them
        // will yield the correct information. When we actually
        // detect a GDK window resize, these values will also be
        // updated, so they will be an accurate representation of
        // the window size.
        int targetWidth;
        int targetHeight;
        int targetX;
        int targetY;
        bool targetMaximized;
        bool targetMinimized;

    protected:
        GtkWindow* gtkWindow;
        GtkWidget* vbox;
        WebKitWebView* webView;
        bool topmost;
        gulong deleteCallbackId;
        AutoPtr<GtkMenu> menu; // The window-specific menu.
        AutoPtr<GtkMenu> activeMenu; // This window's active menu 
        AutoPtr<GtkMenu> contextMenu; // The window specific context menu 
        ::GtkMenuBar* nativeMenu; // The widget this window uses for a menu.
        std::string iconPath; // The path to this window's icon
        GtkWidget *inspectorWindow; // This window's web inspector window
    };
}

#endif
