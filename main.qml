import QtQuick 2.12
import QtQuick.Window 2.12
import QtLocation 5.12
import QtQml 2.12
import QtPositioning 5.12


Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("My Maps")


    Plugin {
        id: somePlugin
        name: "mapbox"

        PluginParameter { name: "mapbox.map_id"; value: "mapbox.satellite" }
        PluginParameter { name: "mapbox.access_token"; value: "pk.eyJ1Ijoic3luYXNpdXMiLCJhIjoiY2lnM3JrdmRjMjJ4b3RqbTNhZ2hmYzlkbyJ9.EA86y0wrXX1eo64lJPTepw" }
    }

    Plugin {
        id: otherPlugin
        name: "osm"

        //Component.onCompleted: {
        //    console.debug("supports geocoding", supportsGeocoding());
        //    console.debug("supports mapping", supportsMapping());
        //    console.debug("supports places", supportsPlaces());
        //    console.debug("supports routing", supportsRouting());
        //}
    }

    RouteModel {
        id: routeModel
        plugin: otherPlugin

        autoUpdate: true


        query: RouteQuery {}

        Component.onCompleted: {
            console.log(locationSource.coordinate)
            query.addWaypoint(locationSource.coordinate);
            query.addWaypoint(QtPositioning.coordinate(40.138322756239056, 17.715292842046768));
            routeModel.update();
        }

        onStatusChanged: console.debug("current route model status", status, count, errorString)
    }

    PlaceSearchModel {
        id: searchModel

        plugin: otherPlugin

        searchTerm: "food"
        searchArea: QtPositioning.circle(map.center, 10000)

        Component.onCompleted: update()

        onStatusChanged: console.debug("current search model status", status, count, errorString())
    }

    PositionSource {
        id: src
        updateInterval: 1000
        active: true

        onPositionChanged: {
            var coord = src.position.coordinate;
            console.debug("current position:", coord.latitude, coord.longitude);
        }
    }

    Connections{
        target: locationSource
        onCoordinateChanged:{
            console.log("COORDINATE CHANGED");
        }
    }

    Map {

//        bearing: 90
        state: "navigating"// ? "navigating" : ""
        id: map
        anchors.fill: parent
        gesture.enabled: true
        plugin: Plugin {
            name: "mapboxgl"

            PluginParameter {
                name: "mapboxgl.mapping.use_fbo"
                value: false
            }
            PluginParameter {
                name: "osm.routing.host"
                value: "http://router.project-osrm.org/route/v4/driving/"
            }


            PluginParameter {
                name: "osm.mapping.highdpi_tiles"
                value: true
            }

            PluginParameter {
                name: "mapboxgl.mapping.additional_style_urls"
                value: "mapbox://styles/mapbox/dark-v11"// "mapbox://styles/mapbox/navigation-guidance-night-v3"
            }

            PluginParameter {
                name: "osm.routing.host"
                value: "http://router.project-osrm.org/route/v4/driving/"
            }

            PluginParameter {
                name: "osm.routing.apiversion"
                value: "v4"
            }

        }

        Plugin {
            name: "osm"
        }

        MapItemView {
            model: searchModel
            delegate: MapCircle {
                center: model.place.location.coordinate

                radius: 50
                color: "#aa449944"
                border.width: 0
            }
        }

        MapItemView {
            model: routeModel
            delegate: MapRoute {
                route: routeData
                line.color: "#aa2235fa"
                line.width: 5
                smooth: true
            }
        }

        MapCircle {
            id: currentPosition

            center: src.position.coordinate
            radius: 50
            color: "red"
            border.width: 0
        }


        center: locationSource.coordinate
        zoomLevel: 16

        copyrightsVisible: false

        Component.onCompleted: {
            //map.toCoordinate = Qt.point(52.520008, 13.404954)

            var params = [
                        "country-label-lg", "country-label-md", "country-label-sm",
                        "state-label-lg", "state-label-md", "state-label-sm",
                        "marine-label-lg-pt", "marine-label-lg-ln", "marine-label-md-pt",
                        "marine-label-md-ln", "marine-label-sm-pt", "marine-label-sm-ln",
                        "place-city-lg-n", "place-city-lg-s", "place-city-md-n",
                        "place-city-md-s", "place-city-sm", "place-island", "place-town",
                        "place-village", "place-hamlet", "place-suburb",
                        "place-neighbourhood", "place-islet-archipelago-aboriginal",
                        "airport-label", "poi-scalerank1", "place-residential",
                        "water-label", "water-label-sm","poi-scalerank2",
                        "motorway-junction", "poi-scalerank3", "poi-scalerank4-l1",
                        "poi-scalerank4-l15", "waterway-label"
                    ]

            for (var i = 0; i < map.supportedMapTypes.length; ++i) {
                console.log(map.supportedMapTypes[i].name)
//                if (map.supportedMapTypes[i].name === "mapbox://styles/mapbox/navigation-preview-night-v2") {
                    if (map.supportedMapTypes[i].name === "mapbox://styles/mapbox/navigation-night-v1") {
                    map.activeMapType = map.supportedMapTypes[i]

                    return
                }
            }
            console.log("CURR MAP: " + map.supportedMapTypes[10].name);
            map.activeMapType = map.supportedMapTypes[10];
            return;

            var lang = settingsManager.language
            lang = lang.substr(0, lang.indexOf('_'));
            // mapboxgl doesn't support Polish language
            if (lang === 'pl') {
                lang = 'en'
            }

            var qml = "import QtLocation 5.12; MapParameter {property var layer; property var textField}"
            for (var j = 0; j < params.length; ++j) {
                var param = Qt.createQmlObject(qml, map)
                param.type = "layout"
                param.layer = params[j]
                param.textField = ["get", "name_" + lang]
                map.addMapParameter(param)
            }

            for (var i = 0; i < map.supportedMapTypes.length; ++i) {
                console.log(map.supportedMapTypes[i])
                if (map.supportedMapTypes[i].name === settingsManager.theme.mapStyle) {
                    map.activeMapType = map.supportedMapTypes[i]
                    return
                }
            }
        }

        MapQuickItem {
            id: currentLocation
            sourceItem: Rectangle {
                width: 40
                height: width
                //color: settingsManager.theme.locationMarkShadow
                smooth: true
                radius: width / 2

                Rectangle {
                    anchors.centerIn: parent
                    width: 20
                    height: width
                    //color: settingsManager.theme.locationMarkColor
                    border.width: 2
                    //border.color: settingsManager.theme.locationMarkBorder
                    smooth: true
                    radius: width / 2
                }
            }
            coordinate: locationSource.coordinate
            opacity: 1.0
            anchorPoint: Qt.point(sourceItem.width / 2, sourceItem.height / 2)
        }

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
        }
    }


}
