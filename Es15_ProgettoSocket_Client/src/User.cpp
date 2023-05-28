#include "../lib/kinderc/kinderc.hpp"

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

struct GeolocatedUserData : public UserData {
    FIELD(double, UltimaLatitudine);
    FIELD(double, UltimaLongitudine);
    FIELD(double, UltimaVelocita);
    FIELD(double, UltimaDirezione);
    FIELD(double, UltimaAltitudine);
    FIELD(const char*, DataOraUltimaPosizione);
};