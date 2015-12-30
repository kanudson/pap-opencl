#include <iostream>
#include <vector>
#include <string>

#include "pap_dijkstra.h"

#include <wx/wx.h>
#include <wx/cmdline.h>
#include "PapWindow.h"

class PapApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

private:
    long countVertecies = 0;
    long countEdges = 0;
    long countSeed = 0;
    bool useHelp = false;
    bool noGui = false;
};

wxIMPLEMENT_APP(PapApp);

bool PapApp::OnInit()
{
    //  call base init method
    //  quit if the call fails
    if (!wxApp::OnInit())
        return false;

    PapWindow* frame = new PapWindow();
    frame->Show();
    return true;
}

void PapApp::OnInitCmdLine(wxCmdLineParser & parser)
{
    parser.AddSwitch(wxT("h"), wxT("help"), wxT("displays the help message"));
    parser.AddSwitch(wxT("g"), wxT("nogui"), wxT("disables the wxWdigets Gui and uses the console for output"));

    parser.AddOption(wxT("v"), wxT("vertices"), wxT("number of vertices in the graph"));
    parser.AddOption(wxT("e"), wxT("edges"), wxT("number of edges per vertex"));
    parser.AddOption(wxT("s"), wxT("seed"), wxT("seed for the graph generator"));

    //  overide the switch character from '/' to '-'
    parser.SetSwitchChars(wxT("-"));
}

bool PapApp::OnCmdLineParsed(wxCmdLineParser & parser)
{
    //  check for help and nogui switches
    useHelp = parser.Found(wxT("h"));
    noGui = parser.Found(wxT("g"));

    bool fv = parser.Found(wxString("v"), &countVertecies);
    bool fe = parser.Found(wxString("e"), &countEdges);
    bool fs = parser.Found(wxString("s"), &countSeed);

    if (noGui && !(fv && fe && fs))
        return false;

    return true;
}
