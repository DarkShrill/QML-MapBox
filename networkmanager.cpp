#include "networkmanager.h"

#include <QNetworkConfigurationManager>
#include <QNetworkReply>
#include <QUrlQuery>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QObject>

#include <memory>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(network, "network")

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {
//  mNetworkManager = std::make_unique<QNetworkAccessManager>(this);
  connect(&ncm, &QNetworkConfigurationManager::onlineStateChanged, this,
          [this](bool online) {
            if (online) {
              emit networkReady();
            }
          });
  if (ncm.isOnline()) {
    emit networkReady();
  }
}

bool NetworkManager::isNetworkReady() const { return ncm.isOnline(); }

void NetworkManager::fetchLocation(double latitude, double longitude,
                                   const QString &lang) const {
  if (qIsNaN(latitude) || qIsNaN(longitude)) {
    qCDebug(network) << "Geo coordinates are empty";
    return;
  }

  QUrl url(QStringLiteral("https://api.pickpoint.io/v1/reverse"));
  QUrlQuery query;
  query.addQueryItem("lat", QString::number(latitude, 0, 'f'));
  query.addQueryItem("lon", QString::number(longitude, 0, 'f'));
  query.addQueryItem("key", mPickpointApiKey);
  query.addQueryItem("accept-language", lang);
  url.setQuery(query);
  qCDebug(network) << "submitting request";

//  auto rep = mNetworkManager->get(QNetworkRequest(url));
  QNetworkRequest req;
  req.setUrl(url);
  QNetworkAccessManager *networkManager = new QNetworkAccessManager();
  auto rep = networkManager->get(req);
  connect(rep, &QNetworkReply::finished, this,
          [this, rep]() { processGeoData(rep); });
}

void NetworkManager::processGeoData(QNetworkReply *networkReply) const {
  if (!networkReply) {
    qCWarning(network) << "Network reply is nullptr";
    return;
  }

  networkReply->deleteLater();
  if (networkReply->error() == QNetworkReply::NoError) {
    QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
    //qDebug() << document;
    QJsonObject rootObject = document.object();
    QJsonValue addressValue = rootObject.value(QStringLiteral("address"));
    if (!addressValue.isObject()) {
      qCWarning(network) << "Reply with geo data has unexpected format";
      return;
    }
    QJsonObject addressObject = addressValue.toObject();
    if (!addressObject.contains("city") || !addressObject.contains("country")) {
      qCWarning(network) << "Reply with geo data has unexpected format";
      return;
    }
    QString location = QStringLiteral("%1, %2").arg(
        addressObject.value(QStringLiteral("city")).toString(),
        addressObject.value(QStringLiteral("country")).toString());
    qCDebug(network) << "got location: " << location;
    emit locationFetched(location);
  } else {
    qCWarning(network) << "Network error has occurred" << networkReply->error();
  }
}

//void NetworkManager::fetchWeatherInfo(double latitude, double longitude) const {
//  qCDebug(network) << "refreshing weather";
//  fetchCurrentWeather(latitude, longitude);
//  fetchForecastWeather(latitude, longitude);
//}

//void NetworkManager::fetchCurrentWeather(double latitude,
//                                         double longitude) const {
//  QUrl url("https://api.openweathermap.org/data/2.5/weather");
//  QUrlQuery query;
//  query.addQueryItem("lat", QString::number(latitude, 'f', 6));
//  query.addQueryItem("lon", QString::number(longitude, 'f', 6));
//  query.addQueryItem("units", "metric");
//  query.addQueryItem("APPID", mOpenweathermapApiKey);
//  url.setQuery(query);

//  auto rep = mNetworkManager->get(QNetworkRequest(url));
//  connect(rep, &QNetworkReply::finished, this,
//          [this, rep]() { processWeatherData(rep); });
//}

//void NetworkManager::fetchForecastWeather(double latitude,
//                                          double longitude) const {
//  QUrl url("https://api.openweathermap.org/data/2.5/forecast");
//  QUrlQuery query;
//  query.addQueryItem("lat", QString::number(latitude, 'f', 6));
//  query.addQueryItem("lon", QString::number(longitude, 'f', 6));
//  query.addQueryItem("units", "metric");
//  query.addQueryItem("cnt", "5");
//  query.addQueryItem("APPID", mOpenweathermapApiKey);
//  url.setQuery(query);

//  auto rep = mNetworkManager->get(QNetworkRequest(url));
//  connect(rep, &QNetworkReply::finished, this,
//          [this, rep]() { processWeatherForecastData(rep); });
//}

//void NetworkManager::processWeatherData(QNetworkReply *networkReply) const {
//  if (!networkReply) {
//    qCWarning(network) << "Network reply is nullptr";
//    return;
//  }

//  networkReply->deleteLater();
//  if (networkReply->error() == QNetworkReply::NoError) {
//    QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
//    QJsonObject rootObject = document.object();
//    qint64 timestamp = rootObject.value(QStringLiteral("dt")).toInt();
//    QJsonValue weatherValue = rootObject.value(QStringLiteral("weather"));
//    if (!weatherValue.isArray()) {
//      qCWarning(network) << "Reply with weather data has unexpected format";
//      return;
//    }
//    QJsonArray weatherArray = weatherValue.toArray();
//    if (weatherArray.isEmpty()) {
//      qCWarning(network) << "Reply with weather data has unexpected format";
//      return;
//    }

//    QJsonObject weatherObject = weatherArray.first().toObject();
//    QString description =
//        weatherObject.value(QStringLiteral("main")).toString();
//    QString icon =
//        iconFromCode(weatherObject.value(QStringLiteral("icon")).toString());

//    QJsonValue mainWeatherValue = rootObject.value(QStringLiteral("main"));
//    if (!mainWeatherValue.isObject()) {
//      qCWarning(network) << "Reply with weather data has unexpected format";
//      return;
//    }
//    QJsonObject mainObject = mainWeatherValue.toObject();
//    float temperature =
//        static_cast<float>(mainObject.value(QStringLiteral("temp")).toDouble());
//    float pressure = static_cast<float>(
//        mainObject.value(QStringLiteral("pressure")).toDouble());
//    int humidity = mainObject.value(QStringLiteral("humidity")).toInt();

//    QJsonValue windValue = rootObject.value(QStringLiteral("wind"));
//    if (!windValue.isObject()) {
//      qCWarning(network) << "Reply with weather data has unexpected format";
//      return;
//    }
//    QJsonObject windObject = windValue.toObject();
//    float windSpeed = static_cast<float>(
//        windObject.value(QStringLiteral("speed")).toDouble());

////    emit weatherFetched({temperature, pressure, humidity, windSpeed,
////                         description, icon, timestamp});
//  } else {
//    qCWarning(network) << "Network error has occurred" << networkReply->error();
//  }
//}

//void NetworkManager::processWeatherForecastData(
//    QNetworkReply *networkReply) const {
//  if (!networkReply) {
//    qCWarning(network) << "Network reply is nullptr";
//    return;
//  }

//  networkReply->deleteLater();
//  if (networkReply->error() == QNetworkReply::NoError) {
//    QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
//    QJsonObject rootObject = document.object();
//    QJsonValue listValue = rootObject.value(QStringLiteral("list"));
//    if (!listValue.isArray()) {
//      qCWarning(network)
//          << "Reply with weather forecast data has unexpected format";
//      return;
//    }
//    QJsonArray weatherArray = listValue.toArray();
//    if (weatherArray.isEmpty()) {
//      qCWarning(network)
//          << "Reply with weather forecast data has unexpected format";
//      return;
//    }

//    QVector<WeatherInfo> weatherForecast;
//    weatherForecast.reserve(weatherArray.size());
//    for (const auto itemValue : weatherArray) {
//      QJsonObject itemObject = itemValue.toObject();
//      qint64 timestamp = itemObject.value(QStringLiteral("dt")).toInt();
//      QJsonValue weatherValue = itemObject.value(QStringLiteral("weather"));
//      if (!weatherValue.isArray()) {
//        qCWarning(network)
//            << "Reply with weather forecast data has unexpected format";
//        return;
//      }
//      QJsonArray weatherInfoArray = weatherValue.toArray();
//      if (weatherInfoArray.isEmpty()) {
//        qCWarning(network)
//            << "Reply with weather forecast data has unexpected format";
//        return;
//      }

//      QJsonObject weatherObject = weatherInfoArray.first().toObject();
//      QString description =
//          weatherObject.value(QStringLiteral("main")).toString();
//      QString icon =
//          iconFromCode(weatherObject.value(QStringLiteral("icon")).toString());

//      QJsonValue mainValue = itemObject.value(QStringLiteral("main"));
//      if (!mainValue.isObject()) {
//        qCWarning(network)
//            << "Reply with weather forecast data has unexpected format";
//        return;
//      }
//      QJsonObject mainObject = mainValue.toObject();
//      float temperature = static_cast<float>(
//          mainObject.value(QStringLiteral("temp")).toDouble());
//      float pressure = static_cast<float>(
//          mainObject.value(QStringLiteral("pressure")).toDouble());
//      int humidity = mainObject.value(QStringLiteral("humidity")).toInt();

//      QJsonValue windValue = itemObject.value(QStringLiteral("wind"));
//      if (!windValue.isObject()) {
//        qCWarning(network)
//            << "Reply with weather forecast data has unexpected format";
//        return;
//      }
//      QJsonObject windObject = windValue.toObject();
//      float windSpeed = static_cast<float>(
//          windObject.value(QStringLiteral("speed")).toDouble());
////      weatherForecast.append({temperature, pressure, humidity, windSpeed,
////                              description, icon, timestamp});
//    }

//    emit weatherForecastFetched(weatherForecast);
//  } else {
//    qCWarning(network) << "Network error has occurred" << networkReply->error();
//  }
//}

QString NetworkManager::iconFromCode(const QString &iconCode) const {
  if (iconCode == "01d" || iconCode == "01n") {
    return QStringLiteral("qrc:/resources/weather/clear_sky.svg");
  }
  if (iconCode == "02d" || iconCode == "02n") {
    return QStringLiteral("qrc:/resources/weather/few_clouds.svg");
  }
  if (iconCode == "03d" || iconCode == "03n") {
    return QStringLiteral("qrc:/resources/weather/clouds.svg");
  }
  if (iconCode == "04d" || iconCode == "04n") {
    return QStringLiteral("qrc:/resources/weather/clouds.svg");
  }
  if (iconCode == "09d" || iconCode == "09n") {
    return QStringLiteral("qrc:/resources/weather/shower_rain.svg");
  }
  if (iconCode == "10d" || iconCode == "10n") {
    return QStringLiteral("qrc:/resources/weather/rain.svg");
  }
  if (iconCode == "11d" || iconCode == "11n") {
    return QStringLiteral("qrc:/resources/weather/thunderstorm.svg");
  }
  if (iconCode == "13d" || iconCode == "13n") {
    return QStringLiteral("qrc:/resources/weather/snow.svg");
  }
  if (iconCode == "50d" || iconCode == "50n") {
    return QStringLiteral("qrc:/resources/weather/mist.svg");
  }

  return {};
}
