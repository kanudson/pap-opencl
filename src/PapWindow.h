#ifndef PapDialog_h__
#define PapDialog_h__

#include "cppcl.hpp"

#include <wx/wx.h>

enum
{
    BTN_GENERATE_GRAPH = wxID_HIGHEST,
    PAPEV_GENERATE_GRAPH_DONE,
    PAPEV_TIMER
};


class PapWindow : public wxFrame 
{
public:
    PapWindow();

    //  generate graph button pressed
    void GenerateGraph(wxCommandEvent& ev);
    void GenerateDone(wxCommandEvent& ev);
    void OnTimer(wxTimerEvent& ev);

    //  called, when input in text boxes or the check box changed
    void GraphConfigChanged(wxCommandEvent& ev);

private:
    //  Input for graph config
    wxTextCtrl* tcVertexCount = nullptr;
    wxTextCtrl* tcEdgePerVec  = nullptr;
    wxTextCtrl* tcStartVertex = nullptr;
    wxTextCtrl* tcEndVertex   = nullptr;
    wxCheckBox* chWeighted    = nullptr;
    wxButton*   btnGenerate   = nullptr;

    void DisableGraphInputs();
    void EnableGraphInputs();

    bool generating = false;
    wxGauge* progressBar = nullptr;
    wxStaticText* stChangedText = nullptr;
    wxTimer* timer = nullptr;
};


#endif // PapDialog_h__
