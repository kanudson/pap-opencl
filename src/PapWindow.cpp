#include "PapWindow.h"


namespace
{
const char* PAP_DIALOG_TITLE = "Wegfindung mit OpenCL";

const char* DEFAULT_VECCOUNT  = "50000000";
const char* DEFAULT_EDGECOUNT = "8";
const char* DEFAULT_STARTVEC  = "0";
const char* DEFAULT_ENDVEC    = "10000";
}

PapWindow::PapWindow()
    :wxFrame(nullptr, wxID_ANY, PAP_DIALOG_TITLE)
{
    timer = new wxTimer(this, PAPEV_TIMER);

    //  Create Group Box for Graph Configuration
    //  uses a Box Sizer with 2 columns
    auto* st = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Graph configuration"));
    auto gs = new wxGridSizer(2);
    tcVertexCount = new wxTextCtrl(this, wxID_ANY, DEFAULT_VECCOUNT);
    tcEdgePerVec  = new wxTextCtrl(this, wxID_ANY, DEFAULT_EDGECOUNT);
    tcStartVertex = new wxTextCtrl(this, wxID_ANY, DEFAULT_STARTVEC);
    tcEndVertex   = new wxTextCtrl(this, wxID_ANY, DEFAULT_ENDVEC);
    chWeighted    = new wxCheckBox(this, wxID_ANY, wxT("create a weighted graph"));
    wxStaticText* text1 = new wxStaticText(this, wxID_ANY, wxT("vertex count"));
    wxStaticText* text2 = new wxStaticText(this, wxID_ANY, wxT("edges per vertex"));
    wxStaticText* text3 = new wxStaticText(this, wxID_ANY, wxT("start vertex"));
    wxStaticText* text4 = new wxStaticText(this, wxID_ANY, wxT("end vertex"));
    wxStaticText* text5 = new wxStaticText(this, wxID_ANY, wxT(" "));
    btnGenerate = new wxButton(this, BTN_GENERATE_GRAPH, wxT("Generate Graph"));
    stChangedText = new wxStaticText(this, wxID_ANY, wxT(">>> Graph config has changed <<<"),
                                     wxDefaultPosition, wxDefaultSize,
                                     wxALIGN_CENTER_HORIZONTAL);
    stChangedText->SetForegroundColour(*wxRED);
    progressBar = new wxGauge(this, wxID_ANY, 100);
    progressBar->Disable();

    wxSizerFlags flags;
    flags.Center().Top().Expand().Border(wxALL, 5);
    gs->Add(text1, flags);
    gs->Add(tcVertexCount, flags);
    gs->Add(text2, flags);
    gs->Add(tcEdgePerVec, flags);
    gs->Add(text3, flags);
    gs->Add(tcStartVertex, flags);
    gs->Add(text4, flags);
    gs->Add(tcEndVertex, flags);
    gs->Add(text5, flags);
    gs->Add(chWeighted, flags);
    st->Add(gs);
    st->Add(btnGenerate, flags);
    st->Add(progressBar, flags);
    st->Add(stChangedText, flags);

    //  Bind events
    Bind(wxEVT_TIMER, &PapWindow::OnTimer, this);
    btnGenerate->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PapWindow::GenerateGraph, this);
    tcVertexCount->Bind(wxEVT_TEXT, &PapWindow::GraphConfigChanged, this);
    tcEdgePerVec->Bind(wxEVT_TEXT, &PapWindow::GraphConfigChanged, this);
    //  start and end vertex are only relevant for runtime, not for graph creation
    //tcStartVertex->Bind(wxEVT_TEXT, &PapWindow::GraphConfigChanged, this);
    //tcEndVertex->Bind(wxEVT_TEXT, &PapWindow::GraphConfigChanged, this);
    chWeighted->Bind(wxEVT_CHECKBOX, &PapWindow::GraphConfigChanged, this);

    auto* vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(st);
    SetBackgroundColour(*wxWHITE);
    SetSizerAndFit(vbox);
    Center();
}

void PapWindow::GenerateGraph(wxCommandEvent & ev)
{
    generating = true;

    btnGenerate->Disable();
    stChangedText->Hide();
    progressBar->Enable();
    progressBar->Pulse();
    DisableGraphInputs();

    //  fire up the creation task and notifier
    //  don't wait for completion, rather get notified when all is done
    timer->Start(2000, true);
}

void PapWindow::GenerateDone(wxCommandEvent & ev)
{
    //  notification that all _is_ actually done
}

void PapWindow::OnTimer(wxTimerEvent & ev)
{
    stChangedText->Hide();
    progressBar->SetValue(0);
    progressBar->Disable();
    generating = false;
    EnableGraphInputs();
}

void PapWindow::GraphConfigChanged(wxCommandEvent & ev)
{
    stChangedText->Show();
    btnGenerate->Enable();
}

void PapWindow::DisableGraphInputs()
{
    tcVertexCount->Disable();
    tcEdgePerVec->Disable();
    tcStartVertex->Disable();
    tcEndVertex->Disable();
    chWeighted->Disable();
}

void PapWindow::EnableGraphInputs()
{
    tcVertexCount->Enable();
    tcEdgePerVec->Enable();
    tcStartVertex->Enable();
    tcEndVertex->Enable();
    chWeighted->Enable();
}
