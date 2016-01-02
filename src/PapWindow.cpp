#include "PapWindow.h"
#include "cppcl.hpp"
#include "pap_dijkstra.h"

#include <future>

namespace
{
const char* PAP_DIALOG_TITLE = "OpenCL stuff";
#ifdef _DEBUG
const char* DEFAULT_VECCOUNT = "50000";
#else
const char* DEFAULT_VECCOUNT = "50000000";
#endif

const char* DEFAULT_EDGECOUNT = "8";
const char* DEFAULT_STARTVEC  = "0";
const char* DEFAULT_ENDVEC    = "10000";
}

wxDEFINE_EVENT(GraphCreated, wxCommandEvent);

PapWindow::PapWindow()
    :wxFrame(nullptr, wxID_ANY, PAP_DIALOG_TITLE)
{
    runlog = new wxTextCtrl(this, wxID_ANY, wxT(""),
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);

    //  Create Group Box for Graph Configuration
    //  uses a Box Sizer with 2 columns
    auto* st = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Graph configuration"));
    auto gs = new wxGridSizer(2);
    tcSeed        = new wxTextCtrl(this, wxID_ANY, wxT("0"));
    tcVertexCount = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(DEFAULT_VECCOUNT));
    tcEdgePerVec  = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(DEFAULT_EDGECOUNT));
    tcStartVertex = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(DEFAULT_STARTVEC));
    tcEndVertex   = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(DEFAULT_ENDVEC));
    chWeighted    = new wxCheckBox(this, wxID_ANY, wxT("create a weighted graph"));
    wxStaticText* text0 = new wxStaticText(this, wxID_ANY, wxT("seed"));
    wxStaticText* text1 = new wxStaticText(this, wxID_ANY, wxT("vertex count"));
    wxStaticText* text2 = new wxStaticText(this, wxID_ANY, wxT("edges per vertex"));
    wxStaticText* text3 = new wxStaticText(this, wxID_ANY, wxT("start vertex"));
    wxStaticText* text4 = new wxStaticText(this, wxID_ANY, wxT("end vertex"));
    wxStaticText* text5 = new wxStaticText(this, wxID_ANY, wxT(" "));
    btnGenerate = new wxButton(this, BTN_GENERATE_GRAPH, wxT("Generate Graph"));
    stMemorySize = new wxStaticText(this, wxID_ANY, wxT(""),
                                    wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_CENTER_HORIZONTAL);
    stChangedText = new wxStaticText(this, wxID_ANY, wxT(">>> Graph config has changed <<<"),
                                     wxDefaultPosition, wxDefaultSize,
                                     wxALIGN_CENTER_HORIZONTAL);
    stChangedText->SetForegroundColour(*wxRED);
    progressBar = new wxGauge(this, wxID_ANY, 100);
    progressBar->Disable();

    wxSizerFlags flags;
    flags.Center().Top().Expand().Border(wxALL, 2);
    gs->Add(text0, flags);
    gs->Add(tcSeed, flags);
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
    st->Add(gs, flags);
    st->Add(stMemorySize, flags);
    st->Add(btnGenerate, flags);
    st->Add(progressBar, flags);
    st->Add(stChangedText, flags);

    RecalcMemorySize();
    sizerDevices = new wxStaticBoxSizer(wxVERTICAL, this, wxT("OpenCL Platforms"));
    sizerRuntime = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Run stuff"));
    runselector = new wxCheckListBox(this, wxID_ANY);
    runbutton = new wxButton(this, wxID_ANY, "run on selected devices");
    runbutton->Disable();
    sizerRuntime->Add(runselector, flags);
    sizerRuntime->Add(runbutton, flags);
    FindOpenCLDevices();

    //  Bind events
    Bind(GraphCreated, &PapWindow::OnGraphCreated, this);
    btnGenerate->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PapWindow::GenerateGraph, this);
    tcVertexCount->Bind(wxEVT_TEXT, &PapWindow::GraphConfigChanged, this);
    tcEdgePerVec->Bind(wxEVT_TEXT, &PapWindow::GraphConfigChanged, this);
    chWeighted->Bind(wxEVT_CHECKBOX, &PapWindow::GraphConfigChanged, this);
    runbutton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PapWindow::RunPathfinding, this);

    auto* hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->Add(st, flags);
    hbox->Add(sizerDevices, flags);
    hbox->Add(sizerRuntime, flags);

    wxSizerFlags flagse(10);
    flagse.Center().Top().Expand().Border(wxALL, 2);

    auto* vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(hbox, flagse);
    vbox->Add(runlog, flagse);
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

    wxString strseed   = tcSeed->GetLineText(0);
    wxString strvcount = tcVertexCount->GetLineText(0);
    wxString strecount = tcEdgePerVec->GetLineText(0);

    unsigned long seed, vcount, ecount;
    strseed.ToULong(&seed);
    strvcount.ToULong(&vcount);
    strecount.ToULong(&ecount);
    
    if (graphdata)
        delete graphdata;

    //  create graph data in an extra thread
    //  gui stays responsive!
    std::packaged_task<void()> tsk([&]()
    {
        //  TODO: put graphdata assignment in main thread
        //  send the ptr with the event as client data?!
        graphdata = new GraphData(vcount, ecount, seed);
        auto* ev = new wxCommandEvent(GraphCreated);
        QueueEvent(ev);
    });
    std::thread thrd(std::move(tsk));
    thrd.detach();
}

void PapWindow::OnGraphCreated(wxCommandEvent & ev)
{
    //  notification that all _is_ actually done
    Done();
}

void PapWindow::GraphConfigChanged(wxCommandEvent & ev)
{
    stChangedText->Show();
    btnGenerate->Enable();
    RecalcMemorySize();
}

void PapWindow::RunPathfinding(wxCommandEvent & ev)
{
    int count;
    wxArrayInt selection;
    count = runselector->GetCheckedItems(selection);

    //  clear text box
    runlog->Clear();

    //  TODO: run on a separate thread?
    //  needs message passing to fill the output log :(
    for (int i = 0; i < count; ++i)
    {
        int id = selection.Item(i);
        CLDEVICE_CLIENTDATA* data = static_cast<CLDEVICE_CLIENTDATA*>(runselector->GetClientData(id));

        wxString devName = data->device.getInfo<CL_DEVICE_NAME>();
        wxString devType = (data->device.getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_CPU) ? "CPU" : "GPU";
        wxString devVendor = data->device.getInfo<CL_DEVICE_VENDOR>();
        wxString selname = wxString::Format(wxT("%s (%s) %s"), devVendor, devType, devName);

        std::ostream ss(runlog);
        ss << "running on " << selname << "\n";
        cl::Context context(data->device);

        try
        {
            runBreadthFirstSearch(context, data->device, *graphdata, 0, 100, ss);
        }
        catch (cl::Error err)
        {
            ss << err.what() << std::endl;
            ss << err.err() << std::endl;
            ss << "stoping execution on " << devName << std::endl;
        }
        ss << "\n";
    }
}

void PapWindow::DisableGraphInputs()
{
    tcSeed->Disable();
    tcVertexCount->Disable();
    tcEdgePerVec->Disable();
    tcStartVertex->Disable();
    tcEndVertex->Disable();
    chWeighted->Disable();
}

void PapWindow::EnableGraphInputs()
{
    tcSeed->Enable();
    tcVertexCount->Enable();
    tcEdgePerVec->Enable();
    tcStartVertex->Enable();
    tcEndVertex->Enable();
    chWeighted->Enable();
}

void PapWindow::Done()
{
    stChangedText->Hide();
    progressBar->SetValue(0);
    progressBar->Disable();
    generating = false;
    EnableGraphInputs();
    runbutton->Enable();
}

void PapWindow::RecalcMemorySize()
{
    if (!tcVertexCount || !tcEdgePerVec || !chWeighted)
        return;

    wxString vcount = tcVertexCount->GetLineText(0);
    wxString ecount = tcEdgePerVec->GetLineText(0);
    bool weighted   = chWeighted->IsChecked();

    unsigned long long vc, ec;
    if (!vcount.ToULongLong(&vc) || !ecount.ToULongLong(&ec))
        return;

    int32_t sVertexMB = ((vc / 1024) / 1024) * sizeof(cl_uint);
    int32_t sEdgeMB   = sVertexMB * ec;
    int32_t sizeMB = sVertexMB + sEdgeMB + (sVertexMB * 5);
    wxString usage = wxString::Format(wxT("Memory usage: ~%iMB (%iMB + %iMB + [5 x %iMB]"),
                                      sizeMB, sVertexMB, sEdgeMB, sVertexMB);
    stMemorySize->SetLabelText(usage);
}

void PapWindow::FindOpenCLDevices()
{
    //  empty sizer, delete all child windows
    sizerDevices->Clear(true);

    wxSizerFlags flags(0);
    flags.Top().Expand().Border(wxALL, 5);

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    //  iterate through all platforms
    for (auto platform : platforms)
    {
        wxString strVendor  = platform.getInfo<CL_PLATFORM_VENDOR>();
        wxString strName    = platform.getInfo<CL_PLATFORM_NAME>();
        wxString strVersion = platform.getInfo<CL_PLATFORM_VERSION>();
        wxString strProfile = platform.getInfo<CL_PLATFORM_PROFILE>();
        
        wxString boxtitle = strVendor + " " + strName;

        wxStaticBoxSizer* subbox = new wxStaticBoxSizer(wxVERTICAL, this, boxtitle);
        wxStaticText* stVersion = new wxStaticText(this, wxID_ANY, strVersion);
        wxStaticText* stProfile = new wxStaticText(this, wxID_ANY, strProfile);
        subbox->Add(stVersion, flags);
        subbox->Add(stProfile, flags);

        //  iteratr through each device on that platform
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, &devices);
        for (auto device : devices)
        {
            wxString devName = device.getInfo<CL_DEVICE_NAME>();
            wxString devType = (device.getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_CPU) ? "CPU" : "GPU";
            wxString devVendor = device.getInfo<CL_DEVICE_VENDOR>();
            wxString devMaxMemAlloc;
            wxString devMaxMemGlobal;
            devMaxMemAlloc << " Max Mem Alloc: " << (device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() / 1024 / 1024) << " MB";
            devMaxMemGlobal << "Max Global Alloc: " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() / 1024 / 1024 << " MB";
            wxString devAvailable = (device.getInfo<CL_DEVICE_AVAILABLE>() == CL_TRUE) ? "Device is available" : "Device is NOT available";

            wxStaticBoxSizer* devbox = new wxStaticBoxSizer(wxVERTICAL, this, devName);
            wxStaticText* stType = new wxStaticText(this, wxID_ANY, devType);
            //wxStaticText* stVendor = new wxStaticText(this, wxID_ANY, devVendor);
            //wxStaticText* stMaxAlloc = new wxStaticText(this, wxID_ANY, devMaxMemAlloc);
            //wxStaticText* stMaxGlobal = new wxStaticText(this, wxID_ANY, devMaxMemGlobal);
            wxStaticText* stAvailable = new wxStaticText(this, wxID_ANY, devAvailable);

            devbox->Add(stType, flags);
            //devbox->Add(stVendor, flags);
            //devbox->Add(stMaxAlloc, flags);
            //devbox->Add(stMaxGlobal, flags);
            devbox->Add(stAvailable, flags);
            subbox->Add(devbox, flags);

            CLDEVICE_CLIENTDATA* data = new CLDEVICE_CLIENTDATA;
            data->device = device;

            wxString selname = wxString::Format(wxT("%s (%s) %s"), devVendor, devType, devName);
            runselector->Append(selname, data);
        }

        sizerDevices->Add(subbox), flags;
    }

    Fit();
}
