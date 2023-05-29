#include "../lib/kinderc/kinderc.hpp"
#include "controller.cpp"

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

    Memory::Grow(48);

    JavaScript::LogCommands();

    Application::UseScreens();
    MapControl::Use();

    btnConnectionTest.onclick = btnConnectionTest_Click;
    btnFrmLogin.onclick = frmLogin_Submit;
    btnLogout.onclick = btnLogout_Click;

    dlgServerConnection.ShowModal();
}