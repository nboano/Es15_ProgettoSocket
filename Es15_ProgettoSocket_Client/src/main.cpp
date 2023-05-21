#include "../lib/kinderc/kinderc.hpp"

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

enum UserRole : int {
    CONSEGNE = 0,
    CONTROLLO = 1
};

struct UserData reflective {
    FIELD(const char*, Cognome);
    FIELD(const char*, Nome);
    FIELD(int, Ruolo);
    FIELD(const char*, Status);
    FIELD(const char*, Username);
    FIELD(const char*, Token);
};

void LoadUserBar(UserData);
void ClearUserBar();

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

class Server {
    private:

    static constexpr char json_user_data[512] = "";

    static void _AsyncRequest(const char* method, const char* path, const char* body, void(*success_cb)(Request&), void(*error_cb)(Request&)) {
        Request r;
        r.open(method, (string)"http://" + Address + path);
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
            dlgConnectionStatus.Find("h3").innerText = "Connessione riuscita";
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
                        break;
                    case UserRole::CONTROLLO:
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
            } else {
                ShowStatusModal(JSON::DeserializeObject(r.ToJSONObject()["error"])["message"]);
            }
        }, Application_NetworkError);
    }
};

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

void btnConnectionTest_Click(HTMLElement&) {
    strcpy((char*)Server::Address, txtServerAddress.value);
    Server::TestConnectionAsync();
}

void btnLogout_Click(HTMLElement&) {
    UserData user_data = Server::GetUserData();

    Server::LogoutAsync(user_data.Username);
}

void frmLogin_Submit(HTMLElement&) {
    strcpy((char*)Server::Address, txtServerAddress.value);
    Server::LoginAsync(txtUsername.value, txtPassword.value);
}

int main() {

    btnConnectionTest.onclick = btnConnectionTest_Click;
    btnFrmLogin.onclick = frmLogin_Submit;
    btnLogout.onclick = btnLogout_Click;

    dlgServerConnection.ShowModal();
}