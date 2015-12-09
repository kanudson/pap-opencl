#include <iostream>
#include <vector>
#include <string>

#include "pap_dijkstra.h"

#include <wx/wx.h>
#include "PapWindow.h"


class PapApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(PapApp);
bool PapApp::OnInit()
{
    PapWindow* frame = new PapWindow();
    frame->Show();
    return true;
}

//////////////////////////////////////////////////////////////////////////
//  'old' main, if you don't want to build with wxWidgets
int main(int argc, char* argv[])
{
    try
    {
        runDijkstra(argc, argv);
    }
    catch (const char* msg)
    {
        std::cerr << msg << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown Error, this is really bad" << std::endl;
    }

    return 0;
}
