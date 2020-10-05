#include <cstdlib>
#include <iostream>
#include <string>
#include <cctype>
#include <curl/curl.h>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/grid.h>

#include "json.hh"
#include "main.hh"



static size_t get_response(void *contents, size_t size, size_t nmemb, void *stream)
{
    size_t real_size = size * nmemb;
    ((std::string *) stream)->append((char *) contents, real_size);
    return real_size;
}


static void print_response(const std::string& response, wxGrid *grid)
{
    int num_rows = grid->GetNumberRows();
    if (num_rows > 0)
        grid->DeleteRows(0, num_rows);

    nlohmann::json j = nlohmann::json::parse(response);

    int line_num = 0;

    for (auto& el : j["result"]["schedules"].items())
    {
        std::string code;
        if (el.value().contains("code"))
        {
            code = el.value()["code"];
            code.erase(std::remove(code.begin(), code.end(), '"'), code.end());
        }

        std::string time = el.value()["message"];
        time.erase(std::remove(time.begin(), time.end(), '"'), time.end());

        std::string dest = el.value()["destination"];
        dest.erase(std::remove(dest.begin(), dest.end(), '"'), dest.end());

        grid->AppendRows();
        grid->SetCellValue(line_num, 0, code);
        grid->SetCellValue(line_num, 1, time);
        grid->SetCellValue(line_num, 2, dest);

        line_num++;
    }
}


bool App::OnInit()
{
    // Initialize Curl
    if (curl_global_init(CURL_GLOBAL_ALL) != 0)
        exit(EXIT_FAILURE);

    Frame *frame = new Frame("RATP Timetables", wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    return true;
}


Frame::Frame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    type_combo_box = new wxComboBox(this, ID_TYPE_COMBOBOX, "", wxPoint(5, 5), wxSize(130, 35));
    type_combo_box->Append("RER");
    type_combo_box->Append("Metro");

    line_combo_box = new wxComboBox(this, ID_LINE_COMBOBOX, "", wxPoint(140, 5), wxSize(100, 35));
    station_combo_box = new wxComboBox(this, ID_STATION_COMBOBOX, "", wxPoint(250, 5), wxSize(300, 35));

    grid_aller = new wxGrid(this, ID_GRID_ALLER, wxPoint(5, 100), wxSize(801, 200));
    grid_aller->CreateGrid(0, 3);
    grid_aller->SetColSize(0, 100);
    grid_aller->SetColSize(1, 180);
    grid_aller->SetColSize(2, 520);
    grid_aller->SetColLabelSize(1);
    grid_aller->SetRowLabelSize(1);

    grid_retour = new wxGrid(this, ID_GRID_RETOUR, wxPoint(5, 350), wxSize(801, 200));
    grid_retour->CreateGrid(0, 3);
    grid_retour->SetColSize(0, 100);
    grid_retour->SetColSize(1, 180);
    grid_retour->SetColSize(2, 520);
    grid_retour->SetColLabelSize(1);
    grid_retour->SetRowLabelSize(1);

}


void Frame::OnTypeComboBox(wxCommandEvent& event)
{
    line_combo_box->Clear();

    switch (type_combo_box->GetCurrentSelection())
    {
        case 0: // RER
            line_combo_box->Append("A");
            line_combo_box->Append("B");
            line_combo_box->Append("C");
            line_combo_box->Append("D");
            line_combo_box->Append("E");
            break;

        case 1: // Metro
            for (int i = 1; i <= 14; i++)
                line_combo_box->Append(std::to_string(i));
            break;
    }

    station_combo_box->Clear();
}


void Frame::OnLineComboBox(wxCommandEvent& event)
{
    CURL *curl = curl_easy_init();

    // ALLER
    std::string url("https://api-ratp.pierre-grimaud.fr/v4/stations/");

    std::string type(type_combo_box->GetStringSelection().data());
    for (size_t i = 0; i < type.size(); i++)
        type[i] = tolower(type[i]);

    url += type + "s/" + line_combo_box->GetStringSelection();

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_perform(curl);

    // Free session
    curl_easy_cleanup(curl);


    nlohmann::json j = nlohmann::json::parse(response);

    for (auto& el : j["result"]["stations"].items())
    {
        std::string slug = el.value()["slug"];
        slug.erase(std::remove(slug.begin(), slug.end(), '"'), slug.end());

        station_combo_box->Append(slug);
    }
}



void Frame::OnStationComboBox(wxCommandEvent& event)
{
    CURL *curl = curl_easy_init();

    // ALLER
    std::string url_aller("https://api-ratp.pierre-grimaud.fr/v4/schedules/");

    std::string type(type_combo_box->GetStringSelection().data());
    for (size_t i = 0; i < type.size(); i++)
        type[i] = tolower(type[i]);

    url_aller += type + "s/"
              + line_combo_box->GetStringSelection() + "/"
              + station_combo_box->GetStringSelection() + "/A/?_format=json";

    std::string response_aller;

    curl_easy_setopt(curl, CURLOPT_URL, url_aller.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_aller);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_perform(curl);

    // RETOUR
    std::string url_retour("https://api-ratp.pierre-grimaud.fr/v4/schedules/");
    url_retour += type + "s/"
               + line_combo_box->GetStringSelection() + "/"
               + station_combo_box->GetStringSelection() + "/R/?_format=json";

    std::string response_retour;

    curl_easy_setopt(curl, CURLOPT_URL, url_retour.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_retour);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Do request
    curl_easy_perform(curl);

    // Free session
    curl_easy_cleanup(curl);


    print_response(response_aller, grid_aller);
    print_response(response_retour, grid_retour);

}


void Frame::OnExit(wxCommandEvent& event)
{
     // Free Curl
    curl_global_cleanup();

    Close(true);
}


void Frame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("Creator: David Ghiassi", "About RATP Timetables",
                 wxOK | wxICON_INFORMATION);
}
