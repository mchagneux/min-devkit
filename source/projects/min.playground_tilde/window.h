#pragma once 
#include "choc/gui/choc_DesktopWindow.h"
#include "choc/gui/choc_MessageLoop.h"
#include "choc/gui/choc_WebView.h"

class WindowWithWebview
{
public:
    WindowWithWebview(): window({100, 100, 800, 600})
    {

        choc::ui::setWindowsDPIAwareness(); // For Windows, we need to tell the OS we're high-DPI-aware

        // choc::ui::DesktopWindow window ({ 100, 100, 800, 600 });

        window.setWindowTitle ("Hello");
        window.setResizable (true);
        window.setMinimumSize (300, 300);
        window.setMaximumSize (1500, 1200);
        window.windowClosed = [] { choc::messageloop::stop(); };

        choc::ui::WebView::Options webViewOptions; 

        webViewOptions.enableDebugMode = true; 
        webViewOptions.enableDebugInspector = true; 

        webView = std::make_unique<choc::ui::WebView>(webViewOptions); 

        webView->navigate("localhost:5173");

        window.setContent (webView->getViewHandle());
        window.setVisible(false); 

    }

    std::unique_ptr<choc::ui::WebView> webView; 
    choc::ui::DesktopWindow window; 

}; 