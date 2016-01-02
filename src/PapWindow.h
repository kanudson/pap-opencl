#ifndef PapDialog_h__
#define PapDialog_h__

#include "cppcl.hpp"
#include "GraphData.h"

#include <wx/wx.h>

enum
{
    BTN_GENERATE_GRAPH = wxID_HIGHEST,
    PAPEV_GENERATE_GRAPH_DONE,
};

wxDECLARE_EVENT(GraphCreated, wxCommandEvent);
wxDECLARE_EVENT(PathfindingDone, wxCommandEvent);

struct CLDEVICE_CLIENTDATA
{
    cl::Device device;
};

class PapWindow : public wxFrame 
{
public:
    PapWindow();
    PapWindow(uint64_t vertex, uint64_t edges, uint64_t seed);

    //  generate graph button pressed
    void GenerateGraph(wxCommandEvent& ev);
    void OnGraphCreated(wxCommandEvent& ev);

    //  called, when input in text boxes or the check box changed
    void GraphConfigChanged(wxCommandEvent& ev);

    void RunPathfinding(wxCommandEvent& ev);

private:
    //  Input for graph config
    wxTextCtrl* runlog         = nullptr;
    wxTextCtrl* tcSeed         = nullptr;
    wxTextCtrl* tcVertexCount  = nullptr;
    wxTextCtrl* tcEdgePerVec   = nullptr;
    wxTextCtrl* tcStartVertex  = nullptr;
    wxTextCtrl* tcEndVertex    = nullptr;
    wxCheckBox* chWeighted     = nullptr;
    wxButton*   btnGenerate    = nullptr;
    wxStaticText* stMemorySize = nullptr;

    bool generating = false;
    wxGauge* progressBar = nullptr;
    wxStaticText* stChangedText = nullptr;

    void DisableGraphInputs();
    void EnableGraphInputs();
    void Done();

    void RecalcMemorySize();
    void FindOpenCLDevices();

    wxStaticBoxSizer* sizerDevices = nullptr;
    wxStaticBoxSizer* sizerRuntime = nullptr;
    wxCheckListBox* runselector = nullptr;
    wxButton* runbutton = nullptr;

    GraphData* graphdata = nullptr;
};


#endif // PapDialog_h__
