#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class App: public wxApp
{
public:
    virtual bool OnInit();
};


class Frame: public wxFrame
{
public:
    Frame(const wxString& title, const wxPoint& pos, const wxSize& size);

    wxComboBox *type_combo_box;
    wxComboBox *line_combo_box;
    wxComboBox *station_combo_box;
    wxGrid *grid_aller;
    wxGrid *grid_retour;

private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTypeComboBox(wxCommandEvent& evt);
    void OnLineComboBox(wxCommandEvent& event);
    void OnStationComboBox(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_TYPE_COMBOBOX = wxID_HIGHEST + 1,
    ID_LINE_COMBOBOX = wxID_HIGHEST + 2,
    ID_STATION_COMBOBOX = wxID_HIGHEST + 3,
    ID_GRID_ALLER = wxID_HIGHEST + 4,
    ID_GRID_RETOUR = wxID_HIGHEST + 5
};


wxBEGIN_EVENT_TABLE(Frame, wxFrame)
    EVT_MENU(wxID_EXIT,  Frame::OnExit)
    EVT_MENU(wxID_ABOUT, Frame::OnAbout)
    EVT_COMBOBOX(ID_TYPE_COMBOBOX, Frame::OnTypeComboBox)
    EVT_COMBOBOX(ID_LINE_COMBOBOX, Frame::OnLineComboBox)
    EVT_COMBOBOX(ID_STATION_COMBOBOX, Frame::OnStationComboBox)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(App);

