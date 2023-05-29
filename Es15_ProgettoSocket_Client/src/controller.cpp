#pragma once
#include "../lib/kinderc/kinderc.hpp"

#include "User.cpp"
#include "MapControl.cpp"

#define txtServerAddress HTMLInputElement($("#txtServerAddress"))
#define txtUsername HTMLInputElement($("#txtUsername"))
#define txtPassword HTMLInputElement($("#txtPassword"))
#define dlgServerConnection HTMLDialogElement($("#dlgServerConnection"))
#define dlgConnectionStatus HTMLDialogElement($("#dlgConnectionStatus"))
#define btnConnectionTest HTMLButtonElement($("#btnConnectionTest"))
#define btnFrmLogin HTMLButtonElement($("#btnFrmLogin"))
#define btnLogout HTMLButtonElement($("#btnLogout"))
#define frmLogin HTMLElement($("#frmLogin"))
#define topUserBar HTMLElement($("#topUserBar"))
#define statusBar HTMLElement($("#statusBar"))

void LoadUserBar(UserData);
void ClearUserBar();
void ShowStatusModal(const char* error_msg);
void HideStatusModal(void*);
void Application_NetworkError(Request&);
void LoadUserBar(UserData user_data);
void ClearUserBar();

void InitConsegne();
void InitControllo();

int GeolocationInterval = 5000;


class Server {
    private:

    static constexpr char json_user_data[512] = "";

    static void _AsyncRequest(const char* method, const char* path, const char* body, void(*success_cb)(Request&), void(*error_cb)(Request&)) {
        static char url_bf[256];
        strcpy(url_bf, "");
        sprintf(url_bf, "./proxy.php?url=http://%s%s", Address, path);

        Request r;
        r.open(method, url_bf);
        r.onload = success_cb;
        r.onerror = error_cb;
        r.send(body);
    }

    public:
    static constexpr char Address[128] = "";

    static UserData GetUserData() {
        return JSON::DeserializeObjectAs<UserData>(json_user_data);
    }

    static void TestConnectionAsync() {

        dlgConnectionStatus.Find("h3").innerText = "Tentativo connessione in corso...";
        dlgConnectionStatus.ShowModal();

        _AsyncRequest("GET","/", nullptr, [](Request& r) {
            if(r.status >= 200 && r.status <= 299)
                dlgConnectionStatus.Find("h3").innerText = "Connessione riuscita";
            else ShowStatusModal("Errore connessione.");
        }, Application_NetworkError);
        
    }

    static void LoginAsync(const char* username, const char* password) {
        _AsyncRequest("POST","/login", String::Format("%s;%s", username, password), [](Request& r) {
            if(r.status >= 200 && r.status <= 299) {
                dlgServerConnection.Close();

                strcpy((char*)json_user_data, r.ToJSONObject()["data"]);
                UserData user_data = GetUserData();

                switch (user_data.Ruolo)
                {
                    case UserRole::CONSEGNE:
                        location.href = "#consegne";
                        InitConsegne();
                        break;
                    case UserRole::CONTROLLO:
                        location.href = "#controllo";
                        InitControllo();
                        break;
                }

                LoadUserBar(user_data);
            } else {
                ShowStatusModal(JSON::DeserializeObject(r.ToJSONObject()["error"])["message"]);
            }
        }, Application_NetworkError);
    }

    static void LogoutAsync(const char* username) {
        _AsyncRequest("POST","/logout", username, [](Request& r) {
            if(r.status >= 200 && r.status <= 299) {
                ClearUserBar();
                dlgServerConnection.ShowModal();
                location.hash = "";
                location.reload();
            } else {
                ShowStatusModal(JSON::DeserializeObject(r.ToJSONObject()["error"])["message"]);
            }
        }, Application_NetworkError);
    }

    static void UpdateLocationAsync(GeolocationData gd) {
        UserData user_data =  GetUserData();
        _AsyncRequest("POST","/update-location", String::Format("%s;%s;%f;%f;%f;%f;%f", user_data.Username, user_data.Token, gd.Latitude, gd.Longitude, gd.Speed, gd.Heading, gd.Altitude), [](Request& r) {
            if(r.status >= 200 && r.status <= 299) {
                statusBar.innerHTML = JSON::DeserializeObject(r.ToJSONObject()["data"])["Status"];
            } else {
                ShowStatusModal(JSON::DeserializeObject(r.ToJSONObject()["error"])["message"]);
            }
        }, Application_NetworkError);
    }

    static void GetPositionsAsync() {
        UserData user_data =  GetUserData();
        _AsyncRequest("POST", "/get-locations", String::Format("%s;%s", user_data.Username, user_data.Token), [](Request& r) {
            if(r.status >= 200 && r.status <= 299) {
                const char* users_geoloc_json = r.ToJSONObject()["data"];

                List<GeolocatedUserData> lst = JSON::DeserializeArrayAs<GeolocatedUserData>(users_geoloc_json);

                free((void*)users_geoloc_json);

                string rhtml = "";
                for(GeolocatedUserData ud: lst) {
                    rhtml += string::Format("<br>&nbsp;%s %s <b>%s</b> %f %f %s<br><br><iframe src='%s' style='display: block; border: 0; width: 100vw; height: 20vh;'></iframe><br>",ud.Nome, ud.Cognome,ud.Username, ud.UltimaLatitudine, ud.UltimaLongitudine, ud.DataOraUltimaPosizione, MapControl::GetOpenStreetMapEmbedURL(ud.UltimaLatitudine, ud.UltimaLongitudine, 0.1));
                }
                                
                $("#controlloContainer").innerHTML = rhtml;

                // for(GeolocatedUserData ud: lst) {
                //     Console::Write("%s %f %f %s<br>",ud.Username, ud.UltimaLatitudine, ud.UltimaLongitudine, ud.DataOraUltimaPosizione);
                // }

                Console::Write("UPDATED");
            } else {
                ShowStatusModal(JSON::DeserializeObject(r.ToJSONObject()["error"])["message"]);
            }
        }, Application_NetworkError);
    }
};

void ShowStatusModal(const char* error_msg) {
    dlgConnectionStatus.Find("h3").innerHTML = error_msg;
    dlgConnectionStatus.ShowModal();
}

void HideStatusModal(void*) {
    dlgConnectionStatus.Close();
}

void Application_NetworkError(Request&) {
    ShowStatusModal("Errore di rete.");
}

void LoadUserBar(UserData user_data) {

    string s = String::Format("%s %s<br><small><i>%s</i></small>", user_data.Nome, user_data.Cognome, user_data.Username);
    string u_status = user_data.Status;

    topUserBar.innerHTML = s;
    statusBar.innerHTML = u_status;
}

void ClearUserBar() {
    topUserBar.innerHTML = "";
    statusBar.innerHTML = "";
}

void UpdatePositionInfo(GeolocationData gd) {
    $("#currentLatitude").innerText = String(gd.Latitude);
    $("#currentLongitude").innerText = String(gd.Longitude);
    $("#currentSpeed").innerText = String(round(gd.Speed * 3.6));
    $("#currentDirection").innerText = String(round(gd.Heading));
    $("#currentAltitude").innerText = String(round(gd.Altitude));

    MapControl($("#consegneMap")).Update(gd.Latitude, gd.Longitude, 0.1);
}

void RequestPosition(void*);
void DisplayAllPositions(void*);

void PositionChangeHandler(GeolocationData gd) {
    UpdatePositionInfo(gd);
    Server::UpdateLocationAsync(gd);
}

void PositionErrorHandler(GeolocationError ge) {
    alert("Impossibile ottenere le informazioni di geolocalizzazione.");
}

void RequestPosition(void*) {
    Geolocation::GetPosition(PositionChangeHandler, PositionErrorHandler);
}

void DisplayAllPositions(void*) {
    Server::GetPositionsAsync();
}

void InitConsegne() {
    Geolocation::HighAccuracy = true;
    RequestPosition(nullptr);
    setInterval(RequestPosition, GeolocationInterval);
}

void InitControllo() {
    DisplayAllPositions(nullptr);
    setInterval(DisplayAllPositions, GeolocationInterval);
}