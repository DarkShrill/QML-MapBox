#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QObject>
#include <QNetworkAccessManager>
#include <memory>

class WeatherInfo;

class NetworkManager : public QObject {
  Q_OBJECT
 public:
  explicit NetworkManager(QObject *parent = nullptr);
  bool isNetworkReady() const;

  void fetchLocation(double latitude, double longitude,
                     const QString &lang) const;
  void fetchWeatherInfo(double latitude, double longitude) const;

 signals:
  void networkReady();
  void locationFetched(const QString &location) const;
  void weatherFetched(const WeatherInfo &weatherInfo) const;
  void weatherForecastFetched(
      const QVector<WeatherInfo> &weatherForecast) const;

 private:
  void fetchCurrentWeather(double latitude, double longitude) const;
  void fetchForecastWeather(double latitude, double longitude) const;
  void processGeoData(QNetworkReply *networkReply) const;
  void processWeatherData(QNetworkReply *networkReply) const;
  void processWeatherForecastData(QNetworkReply *networkReply) const;

  QString iconFromCode(const QString &iconCode) const;

  std::unique_ptr<QNetworkAccessManager> mNetworkManager;
  std::unique_ptr<QNetworkSession> mNetworkSession;
  const QString mPickpointApiKey = "oVUAxLG_KsBbYo3P2vWt";
  const QString mOpenweathermapApiKey = "656d381444524d4d2c599b91d693b603";
  QNetworkConfigurationManager ncm;
};

#endif  // NETWORKMANAGER_H
