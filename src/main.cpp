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
