#include "../lib/kinderc/kinderc.hpp"

class MapControl : public Control<MapControl> {
    public:

    static const char* GetOpenStreetMapEmbedURL(double lat, double lon, double zoom) {
        zoom = 1 / zoom;
        zoom *= 1e-3;

        static char mbf[128];
        strcpy(mbf, "");
        sprintf(mbf, "https://www.openstreetmap.org/export/embed.html?bbox=%f,%f,%f,%f&layer=mapnik&marker=%f,%f", lon - zoom, lat - zoom, lon + zoom, lat + zoom, lat, lon);
        return mbf;
    }

    ControlInit(MapControl);

    void Update(double latitude, double longitude, double zoomlevel) {
        char bf[200] = "";
        sprintf(bf, "<iframe src='%s' style='display: block; border: 0; width: 100%%; height: 100%%;'></iframe>", GetOpenStreetMapEmbedURL(latitude, longitude, zoomlevel));
        innerHTML = bf;
    }

    string Render() {
        style["display"] = "block";
        style["overflow"] = "hidden";
        return "";
    }
};
